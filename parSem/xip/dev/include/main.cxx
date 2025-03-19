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
filename   : main.cxx
Date       : 10/01/1999
Purpose    : XIP parser
Programmer : Claude ROUX
Reviewer   : 
*/


#if defined(WIN32) & !defined(_WINDLL)
extern char AFFICHAGEDOS;
#endif


#include "parsbin.h"

#define EXTRAIRE 2
void AfficheAide(int h);
extern ostream* flot_erreur;
GlobalParseur* ChoixParseur(char xml,
                            char* grammaire,
							char* generefile,
                            char* ntmfile,
                            char* hmmfile,
                            char* tokenize,
                            char* lookup,
                            char* path,
                            lookup_strategy strategy,
                            char* lookup_flags);
void comparegrammaire(GlobalParseur* g1,GlobalParseur* g2,char*,char* grm1,char* grm2);
char XipSetLabelRuleLimit(int ipar,string num_rule);
void compileautomaton(string nom,string sauve,int taille,char);
void afficheversion();
void KifFinalClean();

#ifdef NTMCONNECTION
void conversionfsttrans(char* fst,char* res);
#endif

extern char AFFICHE_PHRASE_MULTIMOTS;
extern int boucledinference;
extern char modeOutC;
extern char nouvel_algo;
extern GlobalParseur* parseur_courant;
extern float resolution_reussite;
extern float resolution_tentative;
extern float tentative_boucle_dependance;
extern float resolution_quantite;
extern float nb_echec_inference;
extern float nombre_boucle_sequence;
extern float nombre_boucle_marquage;
extern char debugkif;
extern char calcultemps;
extern time_t   tempsbasephrase;
extern int appel_construit_index;
extern char affichage_supplementaire;
extern int appel_inference;
extern int appel_calcul_inference;
extern VECTA<GlobalParseur*> gVecteurParseurs;
extern int reussite_inference;
extern char cryptejustelesregles;
extern int echec_a_1;
extern int erreur_inference_noeud;
extern int erreur_sur_dependance;
extern int nb_dependances;
extern int nombre_boucle_inference;
extern int nombre_derecursivation;
extern int test[9];
extern STVecteur dureeaccumulee;
extern float moyenneapplique;
extern int appelinf;
extern int i_compte;
extern int nb_test_marquage;
extern int VERIF;
extern char affiche_regle_appliquee;
extern char crypte_repertoire[1000];
extern char duplication_crypte;
extern int kif_declencheur;
extern char afficheautomate;
//extern char python_debug;
//extern int num_python_ligne;
//extern int num_fichier_python;
extern time_t   tempsinitial;
extern vector<string> kifarguments;


extern char statistique_interne;
void XipFreeAllGrammars();

char LoadGrmFiles(char* grmname,
             char*  ntm,
             char*  hmm,
             char*  grammar, 
			 char* genere,
             char* tokenize,
             char*  lookup,
             lookup_strategy& strategy,
             char* lookup_flags,
             char& conversion,
             VECTA<char*>& parameters,
             vector<string>& g_definition_var,
			 vector<string>& blockingrule,
			 char premier);


void Usage()
{
    *flot_erreur<<"Usage:" <<Endl;
	*flot_erreur<<"     -help chapter (prints out some information about XIP instruction set. -1 display all)"<<Endl;
    *flot_erreur<<"     -grm file (load a grammar file)"<<Endl;
    *flot_erreur<<"     -kif file (execute a kif program)"<<Endl;
    *flot_erreur<<"     -kifargs arg1 arg2 ... (KIF arguments, should be the last definition on the command line)"<<Endl;
    *flot_erreur<<"     -kiftrigger number (set the garbage trigger)"<<Endl;
    *flot_erreur<<"     -verif (check the syntax of the files, then exit"<<Endl;
    *flot_erreur<<"     -english (comments and commands are in English"<<Endl;
    *flot_erreur<<"     -french (comments and commands are in French"<<Endl;
    *flot_erreur<<"     -string filename (personal string file)"<<Endl;
	*flot_erreur<<"     -loadprobabilities filename (Init probabilities)"<<Endl;
	*flot_erreur<<"     -random (Triggger the random part of probability parsing)"<<Endl;
	*flot_erreur<<"     -variable VARIABLE (activate a IFDEF section in the grammar)"<<Endl;
    *flot_erreur<<"     -text filename (text to parse)"<<Endl;
    *flot_erreur<<"     -dependencies filename (dependencies in input for generation)"<<Endl;
    *flot_erreur<<"     -texts filename (a file that comprises on each line a filename to parse)"<<Endl;
    *flot_erreur<<"     -seek value (starts the analysis of a text file at value)"<<Endl;
    *flot_erreur<<"     -ntm ntmscript"<<Endl;
    *flot_erreur<<"     -ntmonly"<<Endl;
    *flot_erreur<<"     -pathnode (display the node with their complete path)"<<Endl;
	*flot_erreur<<"     -block rulespace (blocks the execution of a given rulespace)"<<Endl;
    *flot_erreur<<"     -hmm hmmfilename"<<Endl;
    *flot_erreur<<"     -time (computes time of analysis)"<<Endl;
    *flot_erreur<<"     -number value (overides field number in parameter file)"<<Endl;
    *flot_erreur<<"     -g (parameter file for a given language)"<<Endl;
    *flot_erreur<<"     -tagger (tagging mode, no parsing)"<<Endl;
    *flot_erreur<<"     -tagging (disambiguation rules are used while parsing)"<<Endl;
    *flot_erreur<<"     -ntagging (disambiguation rules are not used while parsing)"<<Endl;
    *flot_erreur<<"     -a (for a full display of syntactic node information)"<<Endl;
    *flot_erreur<<"     -r (for a reduced display of syntactic node information)"<<Endl;
    *flot_erreur<<"     -tr (for an even more reduced display of syntactic node information)"<<Endl;
    *flot_erreur<<"     -lem (same as -tr but with lemmas instead of surface form)"<<Endl;
    *flot_erreur<<"     -mwdisplay (display the rebuilt string to which multiword rules are applied)"<<Endl;
    *flot_erreur<<"     -cat (display the leaf category. Sould be used AFTER -lem or -tr)"<<Endl;
    *flot_erreur<<"     -fullcat (display the leaf category, with features in [] and lemma as a feature. Sould be used AFTER -lem or -tr)"<<Endl;
    *flot_erreur<<"     -gencat (display the leaf category, with features in [] and lemma as a feature. No Chunk tree displayed"<<Endl;
    *flot_erreur<<"     -gen (display the generated sentence)"<<Endl;
    *flot_erreur<<"     -disj (display the disjunctive sets for incompatible dependencies)"<<Endl;
    *flot_erreur<<"     -nm (display the word number)"<<Endl;
    *flot_erreur<<"     -obn (order by node, the dependencies are displayed according to their first parameter)"<<Endl;
    *flot_erreur<<"     -ord (dependencies are ordered according to their creation)"<<Endl;
    *flot_erreur<<"     -depfeatval (display the value of the feature dependencies)"<<Endl;
    *flot_erreur<<"     -t (display of the syntactic tree)"<<Endl;
    *flot_erreur<<"     -tl number (automatically wraps the syntactic tree at 'number')"<<Endl;
    *flot_erreur<<"     -tc (display of the syntactic tree. Each sub-node under the top node is displayed independently)"<<Endl;
    *flot_erreur<<"     -offset (display offset values for each token)"<<Endl;
    *flot_erreur<<"     -sentence (display the sentence that has been analyzed)"<<Endl;
    *flot_erreur<<"     -sentencenum (work with -sentence, display the sentence number with the sentence itself)"<<Endl;
    *flot_erreur<<"     -ntree (the chunk tree is not displayed)"<<Endl;
    *flot_erreur<<"     -prolog (dependencies are displayed in prolog form)"<<Endl;
    *flot_erreur<<"     -inpututf8 (Uses this flag if the text is in UTF-8 format)"<<Endl;
    *flot_erreur<<"     -outpututf8 (Uses this flag to output the text in UTF-8 format)"<<Endl;    
	*flot_erreur<<"     -testutf8 (Test the UTF8 encoding before applying anything)"<<Endl;
    *flot_erreur<<"     -tokenized (Each line in the input file is a single token only valid with a NTM script)"<<Endl;    
    *flot_erreur<<"     -languagechecking (The parser checks for each sentence whether it belongs to the current grammar language)"<<Endl;
    *flot_erreur<<"     -no_normalization (Does not normalize the translation tags to lowercase)"<<Endl;
#if defined(WIN32)
    *flot_erreur<<"     -dos (Translates characters in DOS format)"<<Endl;
#endif
    *flot_erreur<<"     -intrans filein -outtrans fileout -nbarcs value (Generate an automaton from filein into fileout. -nbarcs defines the number of arcs (optional))"<<Endl;
    *flot_erreur<<"     -trans filein fileout -nbarcs value (Generate an automaton from filein into fileout. -nbarcs defines the number of arcs (optional))"<<Endl;
    *flot_erreur<<"     -f (extraction of the dependencies)"<<Endl;
	*flot_erreur<<"     -displaycode (Display the input as integers)"<<Endl;
    *flot_erreur<<"     -nodisplay (only execution, no output)"<<Endl;
    *flot_erreur<<"     -max nb (maximum number of sentences that will be analyzed"<<Endl;
    *flot_erreur<<"     -sent (only display the sentences with their number as they will be processed"<<Endl;
    *flot_erreur<<"     -trace (generates a trace in the file defined in the configuration file)"<<Endl;
    *flot_erreur<<"     -dependency (generates a trace fils only for the dependency calculus)"<<Endl;
    *flot_erreur<<"     -rules (generates a trace file containing rules and their actual index)"<<Endl;
    *flot_erreur<<"     -automaton (generates a trace file with the rule automata inside)"<<Endl;
    *flot_erreur<<"     -indent (generates a file containing the result of the parse as indented trees)"<<Endl;
    *flot_erreur<<"     -ne  (display a dependency with the list of rules that applied to create it)"<<Endl;
    *flot_erreur<<"     -np (adds the phrase number in the dependency output, does not display the chunk tree)"<<Endl;
    *flot_erreur<<"     -npi (same as -np but also displies an inverted form of the dependencies)"<<Endl;
    *flot_erreur<<"     -feat (display the feature structure for each token in tagger mode)"<<Endl;
    *flot_erreur<<"     -addhmm (adds the hmm features for the non-selected readings after an HMM tagging)"<<Endl;
    *flot_erreur<<"     -math (display or transfers the result of mathematical formulae)"<<Endl;
    *flot_erreur<<"     -mathonly (only display the result of mathematical formulae, it does not display other results)"<<Endl;
    *flot_erreur<<"     -upto  label|num (run the grammar up to a certain label or a rule number)"<<Endl;
    *flot_erreur<<"     -optimize file (generate a file with the optimized dependency rules)"<<Endl;
    *flot_erreur<<"     -xml (the output is displayed as an xml output)"<<Endl;
#ifdef XIPLIBXML
    *flot_erreur<<"     -xmldb alias filename"<<Endl;
    *flot_erreur<<"\t\t   alias is the name through which the database is accessed in rules"<<Endl;
    *flot_erreur<<"\t\t   filename the filename of the database"<<Endl;
    *flot_erreur<<"     -xmltext depth filename (an xml file which will be parsed according to the XMLGUIDE provided by the grammar)"<<Endl;
    *flot_erreur<<"     -xmltexts depth filename (a text file which contains xml filenames which will be parsed according to the XMLGUIDE provided by the grammar)"<<Endl;
    *flot_erreur<<"\t\t   depth is used to split an XML document into chunks according to their depth,"<<Endl;
    *flot_erreur<<"\t\t   it is especially useful to treat large XML document that could not be loaded at once in memory."<<Endl;
    *flot_erreur<<"     -insxml inserts the result of the analysis into the xml file as output"<<Endl;
    *flot_erreur<<"     -outxml inserts the result of the analysis into the xml file as output, the result is then displayed on screen"<<Endl;
    *flot_erreur<<"     -text2xml root encoding filename (creates a new xml file out of a text file with 'root' as the root markup tag and with a certain encoding)"<<Endl;
    *flot_erreur<<"     -resultxmldir path (the path where result files are stored)"<<Endl;
    *flot_erreur<<"     -passageXml (outputs results in passage xml format)"<<Endl;
#endif
    *flot_erreur<<"     -markup (the content of the XML tags being analyzed is displayed on screen)"<<Endl;
    *flot_erreur<<"     -warning (display the unknown tag warnings (lexicon tags that have not been associated with a parser category)"<<Endl;
    *flot_erreur<<"     -debug (display the execution error of XIP instructions)."<<Endl;
    *flot_erreur<<"     -kifdebug (display the execution error of KIF instructions)."<<Endl;
    *flot_erreur<<"     -sem filename (this command is used to create an index file)"<<Endl;
    *flot_erreur<<"\t\t   The filename must contain semantic disambiguation rules."<<Endl;
    *flot_erreur<<"\t\t   More than one file can be indexed in one step."<<Endl;
    *flot_erreur<<"\t\t   The -index must be provided on the command line."<<Endl;
    *flot_erreur<<"     -index filename (this command must be used together with -sem. The filename stores the index computed out of the -sem filenames)"<<Endl;
    *flot_erreur<<"     -p filename (filename comprises new rules and definition to add to the kernel grammar)"<<Endl;
    *flot_erreur<<"     -renum (for each new sentence, word number starts at 0)"<<Endl;
    *flot_erreur<<"     -nrenum (the first word number of a new sentence continues those of the previous sentences)"<<Endl;    
    *flot_erreur<<"     -version (Display the XIP version)"<<Endl;    
    *flot_erreur<<Endl<<"     \"a sentence\" or < file"<<Endl;
}


void affiche_version() {
        string question;

        Whoami(question);
    *flot_erreur<<Endl<<"\t"<<ConversionChaine(NULL,"Copyright © Xerox Corporation 2000-2007")<<Endl
        <<"\t"<<question<<Endl
        <<"\tXerox Research Center Europe"<<Endl
#ifdef NTMCONNECTION
        <<"\t(TOKENIZER, LOOKUP, NTM and HMM included)"<<Endl;
#else
    <<"\t(TOKENIZER, LOOKUP, NTM and HMM NOT included)"<<Endl;
#endif
#ifdef XIPPYTHON
    *flot_erreur<<"\tPython enable"<<Endl;
#endif
#ifdef XIPLIBXML
    *flot_erreur<<"\tXML enable"<<Endl;
#endif
    *flot_erreur<<Endl<<"\tImplementation:"<<Endl
        <<"\t\tClaude Roux"<<Endl
        <<"\t\tAaron Kaplan"<<Endl
		<<"\t\tMatthieu Chartier"<<Endl
        <<Endl<<"\tConception:"<<Endl
        <<"\t\t"<<ConversionChaine(NULL,"Salah Aït-Mokhtar")<<Endl
        <<"\t\tCaroline Brun"<<Endl
        <<"\t\tJean-Pierre Chanod"<<Endl;
    *flot_erreur<<"\t\tCaroline "<<ConversionChaine(NULL,"Hagège")<<Endl
        <<"\t\tAaron Kaplan"<<Endl
        <<"\t\tClaude Roux"<<Endl
        <<"\t\tGilbert Rondeau"<<Endl;
    *flot_erreur<<"\t\t"<<ConversionChaine(NULL,"Frédérique Segond")<<Endl;
    *flot_erreur<<"\t\t"<<ConversionChaine(NULL,"Ágnes Sándor")<<Endl<<Endl;
}


#ifndef LIBXIP
//The output is a TansVectorText object...
int main (int argc, char *argv[])
{
    //This client needs a context, this line is alas mandatory...
    int i, data_pos,fromPipe=0;
    
    if (!isatty(0)) {
        fromPipe=1;
    }

#if defined(WIN32)    
    AFFICHAGEDOS=1;
#endif
    affiche_version();
    
    if (!fromPipe && argc <= 1) {
        Usage();
        *flot_erreur<<Endl<<"Please enter the name of the file to process"<<Endl;        
        return -1;
    }
    	
	char python_debug=NON;
	int num_python_ligne=0;
	int num_fichier_python=0;
	char statistiques=NON;
    InitialisationSignaux();
    long seek_value=-1;
    ushort lePort=0;
    affiche_regle_appliquee=NON;
    lookup_strategy strategy=NO_STRATEGY;
    vector<string> g_definition;
	vector<string> blockingrule;
    char grammaire[1000];
    char path[1000];
	char affichelescodes=NON;
    char extract=0;    
    TVecteur laffichage= AFFICHAGE_COMPLET | AFFICHAGE_LEMME  | AFFICHAGE_SURFACE | AFFICHAGE_ENTREE | AFFICHAGE_CATEGORIE | AFFICHAGE_ARBRE_CHUNK;
    TVecteur tousaffichages=laffichage;
    TVecteur de_tree=0;
    int column=80;
    char p_chemin=0;
    char p_fichier=0;
    char pretraitement=NON;
    TVecteur aff_fonc=AFFICHE_FONCTION;
    char tagger=NON;
    char tagging=OUI;
    char aff_err=NON;
    char afficherResultats=OUI;
	char fusiongrm=NON;
    int fili;
    TVecteur ordreDependance=AFFICHAGE_DEP_NOM;
    typeLangue langue=ANGLAIS;
    VECTA<char*> fileparam;
    VECTA<char*> fileparaminter;
    VECTA<char*> filexml;
    VECTA<char*> alias;
    VECTA<int> profxmltextes;    
	VECTA<int> dependances;    
	VECTA<char*> fichiersem;
    VECTA<int> xmlfileinput;
    VECTA<char*> encodages;
    VECTA<char*> racinebalise;
	vector<string> fichierkifs;
    char fichierindex[1000];
    char err[1000];
    int maxSentence=-1;
    char conversion=NON;
    char repertoireresultat[1000];
    int prof_xmltexts=0;
    char corpus_xmltexts[1000];
    long conversionutf8=0;
    size_t ide;

    char xtdfile[1000];
    char hmmfile[1000];
    char ntmfile[1000];
    char stringfile[1000];
    char label_limite[100];
    char lookupfile[1000];
    char tokenfile[1000];
	char generefile[1000];
    VECTA<char*> grmfile;
    char optimize=NON;
    VECTA<char*> textes;
    char renumerote=OUI;
    char tree=OUI;
    char analyse=OUI;
	char sortiepredef=NON;
    
    label_limite[0]=0;
    corpus_xmltexts[0]=0;
    fichierindex[0]=0;
    hmmfile[0]=0;
    ntmfile[0]=0;
    grammaire[0]=0;
    stringfile[0]=0;
    xtdfile[0]=0;
    repertoireresultat[0]=0;
    lookupfile[0]=0;
	tokenfile[0]=0;
	generefile[0]=0;
    VECTA<char*> TraceFichiersParam;
    char lookup_flags[100];
    lookup_flags[0]=0;
    
    //the language argument is processed
    int arret=0;
    int numberValues=-1;
    data_pos=1;
    char crypte=NON;
    char tracetraits=NON;

	string probafile;
    
    char corpus[1000];
    
    corpus[0]=0;
    
    calcultemps=NON;
    time_t   tempsfinal;
    double   duree;
    char math=NON;
    VECTA<char*> blockingrulespaces;
    
    VERIF=NON;
	long nbarcs=3000000;
    
    
#if defined(WIN32)
    AFFICHAGEDOS=0;
#endif
    
    char fonction_synt=NON;
    char theTrace=0;
    TVecteur affichage_offset=0;
    char inputXML=NON;
    char outputXML=NON;
    char theRules=NON;
    char mode_prolog=NON;
    char genere=NON;
    char traceDependance=NON;
    affichage_supplementaire=0;

	string intrans;
	string outtrans;
	bool b_intrans=false;
	bool b_outtrans=false;
    

    for (i = 1; i <argc ;i++) {

		if (!strcmp(argv[i],"-version")) {
			afficheversion();
			exit(-1);
		}

        if (!strcmp(argv[i],"-l") || !strcmp(argv[i],"-g")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                if (!strcmp(argv[i],"-g"))
                    crypte=OUI;
                strcpy(grammaire,argv[++i]);
                p_fichier=1;
            }
            continue;
        }
        
        if (!strcmp(argv[i],"-index")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else 
                strcpy(fichierindex,argv[++i]);
            continue;
        }
        
        if (!strcmp(argv[i],"-block")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a rulespace"<<Endl;                
                return -1;
            }
            else 
                blockingrulespaces.ajoute(argv[++i]);
            continue;
        }
        

        if (!strcmp(argv[i],"-grm")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                grmfile.ajoute(DupliqueChaine(argv[++i]));
                p_fichier=1;
            }
            continue;
        }


		if (!strcmp(argv[i],"-kif")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else
                fichierkifs.push_back(argv[++i]);
            continue;
        }

		if (!strcmp(argv[i],"-kifargs")) {
			i++;
			while (i<argc)
				kifarguments.push_back(argv[i++]);
			continue;
		}

        if (!strcmp(argv[i],"-kiftrigger")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a numeral"<<Endl;                
                return -1;
            }
            else
                kif_declencheur=atoi(argv[++i]);
            continue;
        }


#ifdef NTMCONNECTION
		if (!strcmp(argv[i],"-convert")) {
            if ((i+2)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                intrans=argv[++i];
				outtrans=argv[++i];
				conversionfsttrans((char*)intrans.c_str(),(char*)outtrans.c_str());
#ifndef _DEBUG
				exit(-1);
#endif
            }
            continue;
        }
#endif
		if (!strcmp(argv[i],"-trans")) {
            if ((i+2)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                intrans=argv[++i];
				b_intrans=true;
				outtrans=argv[++i];
				b_outtrans=true;
            }
            continue;
        }
		
		if (!strcmp(argv[i],"-intrans")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                intrans=argv[++i];
                b_intrans=true;
            }
            continue;
        }

		if (!strcmp(argv[i],"-outtrans")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                outtrans=argv[++i];
                b_outtrans=true;
            }
            continue;
        }

		//Pour afficher les stats de boucles internes
	    if (!strcmp(argv[i],"-loopstats")) {
            statistiques=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-optimize")) {
            optimize=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-outc")) {
            modeOutC=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-output")) {
            sortiepredef=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-verif")) {
            VERIF=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-interrules")) {
            statistique_interne=OUI;
            affiche_regle_appliquee=2;
            continue;
        }
        

        if (!strcmp(argv[i],"-printrules")) {
            statistique_interne=OUI;
            affiche_regle_appliquee=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-tokenized") || !strcmp(argv[i],"-tokenize")) {
            laffichage|=EXECUTE_TOKENIZE;
            continue;
        }
        
#ifdef XIPLIBXML       
        if (!strcmp(argv[i],"-resultxmldir")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else
                strcpy(repertoireresultat,argv[++i]);                            
            continue;
        }
        
        char* charg;
        if (!strcmp(argv[i],"-xmldb")) {
            if ((i+2)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Missing arguments for -xmldb"<<Endl;                
                return -1;
            }
            else {                
                charg=argv[++i];                                
                alias.ajoute(DupliqueChaine(charg));
                charg=argv[++i];
                filexml.ajoute(DupliqueChaine(charg));                
            }            
            continue;
        }
        
        if (!strcmp(argv[i],"-xtd")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else                
                strcpy(xtdfile,argv[++i]);                
            continue;
        }

        if (!strcmp(argv[i],"-upto")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please insert a label or a rule number"<<Endl;                
                return -1;
            }
            else                
                strcpy(label_limite,argv[++i]);                
            continue;
        }

        if (!strcmp(argv[i],"-xmltext")) {

            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                
                int prof=0;         
                charg=argv[++i];
                if (charg[0] >='0' && charg[0]<='9' && charg[1]==0) {
                    prof=atoi(charg);
                    if ((i+1)>=argc) {
                        Usage();
                        *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                        
                        return -1;
                    }
                    charg=argv[++i];
                }

                racinebalise.ajoute(DupliqueChaine("r"));
                encodages.ajoute(DupliqueChaine("e"));
                xmlfileinput.ajoute(OUI);
                profxmltextes.ajoute(prof);                
                textes.ajoute(DupliqueChaine(charg));
            }
            continue;
        }

        if (!strcmp(argv[i],"-xmltexts")) {

            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                
                int prof=0;         
                charg=argv[++i];
                if (charg[0] >='0' && charg[0]<='9' && charg[1]==0) {
                    prof_xmltexts=atoi(charg);
                    if ((i+1)>=argc) {
                        Usage();
                        *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                        
                        return -1;
                    }
                    charg=argv[++i];
                }

                strcpy(corpus_xmltexts,charg);
            }
            continue;
        }

#endif
        
        if (!strcmp(argv[i],"-p")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else
                fileparam.ajoute(DupliqueChaine(argv[++i]));
            continue;
        }
        
        if (!strcmp(argv[i],"-sem")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else
                fichiersem.ajoute(DupliqueChaine(argv[++i]));
            continue;
        }
        
        if (!strcmp(argv[i],"-string")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                strcpy(stringfile,argv[++i]);
                langue=PERSONNEL;
            }
            continue;
        }

        if (!strcmp(argv[i],"-loadprobabilities")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else
                probafile=argv[++i];
            continue;
        }

		if (!strcmp(argv[i],"-variable")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a variable definition"<<Endl;                
                return -1;
            }
            else
				g_definition.push_back(argv[++i]);
            continue;
        }
        
        if (!strcmp(argv[i],"-hmm")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else
                strcpy(hmmfile,argv[++i]);
            continue;
        }
        
        if (!strcmp(argv[i],"-math")) {
            math=OUI;
            continue;
        }
        
#if defined(WIN32)
        if (!strcmp(argv[i],"-dos")) {
            AFFICHAGEDOS=OUI;
            continue;
        }
#endif
        if (!strcmp(argv[i],"-inpututf8")) {
            conversion=OUI;
            conversionutf8 |= ENTREE_UTF8;
            continue;
        }

		if (!strcmp(argv[i],"-lower")) {
            affichage_offset |= ENTREE_EN_MINUSCULE;
            continue;
        }

        
        if (!strcmp(argv[i],"-utf8") || !strcmp(argv[i],"-outpututf8")) {
            conversion=OUI;
            conversionutf8 |= AFFICHAGE_UTF8;
            continue;
        }
        
        if (!strcmp(argv[i],"-mathonly")) {
            math=2;
            continue;
        }
        
        if (!strcmp(argv[i],"-generate")) {
            genere=OUI;
            continue;
        }

		if (!strcmp(argv[i],"-diff")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a directory"<<Endl;                
                return -1;
            }
            else
                strcpy(crypte_repertoire,argv[++i]);

            if (crypte_repertoire[strlen(crypte_repertoire)-1]!=SEP) {
                crypte_repertoire[strlen(crypte_repertoire)]=SEP;
                crypte_repertoire[strlen(crypte_repertoire)+1]=0;
            }

			statistique_interne=OUI;
            fusiongrm=OUI;
            continue;
        }
        
        if (!strcmp(argv[i],"-text")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                racinebalise.ajoute(DupliqueChaine("r"));
                encodages.ajoute(DupliqueChaine("e"));
                xmlfileinput.ajoute(NON);
                profxmltextes.ajoute(0);                
                textes.ajoute(DupliqueChaine(argv[++i]));
            }
            continue;
        }

		if (!strcmp(argv[i],"-dependencies")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else {
                racinebalise.ajoute(DupliqueChaine("r"));
                encodages.ajoute(DupliqueChaine("e"));
                xmlfileinput.ajoute(NON);
                profxmltextes.ajoute(0);  
				dependances.ajoute(1);
                textes.ajoute(DupliqueChaine(argv[++i]));
            }
            continue;
        }
        
        if (!strcmp(argv[i],"-texts")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else
                strcpy(corpus,argv[++i]);
            continue;
        }

        if (!strcmp(argv[i],"-text2xml")) {
            if ((i+3)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Missing arguments"<<Endl;                
                return -1;
            }
            else {
                racinebalise.ajoute(DupliqueChaine(argv[++i]));
                encodages.ajoute(DupliqueChaine(argv[++i]));
                xmlfileinput.ajoute(OUTPUTGENEREXML);
                profxmltextes.ajoute(0);                
                textes.ajoute(DupliqueChaine(argv[++i]));
            }                
            continue;
        }
        
        
        if (!strcmp(argv[i],"-ntm")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a filename"<<Endl;                
                return -1;
            }
            else
                strcpy(ntmfile,argv[++i]);
            continue;
        }
        
        if (!strcmp(argv[i],"-number")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a numeral"<<Endl;                
                return -1;
            }
            else
                numberValues=atoi(argv[++i]);
            continue;
        }

		if (!strcmp(argv[i],"-nbarcs")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a numeral"<<Endl;                
                return -1;
            }
            else
                nbarcs=atoi(argv[++i])*1000;
            continue;
        }
        
        if (!strcmp(argv[i],"-max")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a numeral"<<Endl;                
                return -1;
            }
            else
                maxSentence=atoi(argv[++i])-1;
            continue;
        }
        
        if (!strcmp(argv[i],"-port")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a numeral"<<Endl;                
                return -1;
            }
            else
                lePort=atoi(argv[++i]);
            continue;
        }
        
        if (!strcmp(argv[i],"-x")) {
            afficherResultats=NON;
            continue;
        }

        if (!strcmp(argv[i],"-nodisplay")) {
            afficherResultats=NON;
            continue;
        }
        
        if (!strcmp(argv[i],"-obn")) {
            ordreDependance=AFFICHAGE_DEP_NOEUD;
            continue;
        }
        
        if (!strcmp(argv[i],"-ord")) {
            ordreDependance=AFFICHAGE_DEP_CREATION;
            continue;
        }
        
        if (!strcmp(argv[i],"-ntmonly")) {
            analyse=NON;
            continue;
        }
        
        if (!strcmp(argv[i],"-ixml")) {
            inputXML=OUI;
            continue;
        }
        
        if (!strcmp(argv[i],"-ntree")) {
            tree=NON;
            laffichage&=~AFFICHAGE_ARBRE_CHUNK;
            continue;
        }
        
        if (!strcmp(argv[i],"-prolog")) {
            tree=NON;
            laffichage&=~AFFICHAGE_ARBRE_CHUNK;
            fonction_synt=OUI;
            mode_prolog=OUI;
            continue;
        }
        
        if (!strcmp(argv[i],"-sent")) {
            arret=OUI;
            continue;
        }
        
        
        if (!strcmp(argv[i],"-languagechecking")) {
            affichage_offset|=TESTER_LANGUE;
            continue;
        }

		if (!strcmp(argv[i],"-random")) {
            affichage_offset|=ANALYSE_ALEATOIRE;
            continue;
        }

        if (!strcmp(argv[i],"-offset")) {
            affichage_offset|=AFFICHAGE_OFFSET;
            continue;
        }

        if (!strcmp(argv[i],"-sentencenum")) {
            affichage_offset|=AFFICHAGE_NUMPHRASE;
            continue;
        }

        
        if (!strcmp(argv[i],"-no_normalization")) {
            affichage_offset|=NON_NORMALISATION_TAG;
            continue;
        }

        if (!strcmp(argv[i],"-sentence")) {
            affichage_offset|=AFFICHAGE_PHRASE;
            continue;
        }
        
        if (!strcmp(argv[i],"-nm")) {
            affichage_offset|=AFFICHAGE_NUM_MOT;
            continue;
        }

        if (!strcmp(argv[i],"-pathnode")) {
            affichage_supplementaire=1;
            continue;
        }
        
        if (!strcmp(argv[i],"-time")) {
            calcultemps=OUI;
            continue;
        }
        
        if (!strcmp(argv[i],"-detect")) {
            calcultemps=2;
            continue;
        }
        
        if (!strcmp(argv[i],"-stats")) {
            calcultemps=OUI;
            statistique_interne=OUI;
            continue;
        }
        
        
        if (!strcmp(argv[i],"-renum")) {
            renumerote=OUI;
            continue;
        }
        
        if (!strcmp(argv[i],"-nrenum")) {
            renumerote=NON;
            continue;
        }
//Les affichages
        if (!strcmp(argv[i],"-a")) {
            laffichage=AFFICHAGE_COMPLET | AFFICHAGE_LEMME  | AFFICHAGE_SURFACE | AFFICHAGE_ENTREE | AFFICHAGE_CATEGORIE | AFFICHAGE_ARBRE_CHUNK;
            continue;
        }

        if (!strcmp(argv[i],"-r")) {
            laffichage=laffichage & ~tousaffichages;
            laffichage|=AFFICHAGE_COMPLET | AFFICHAGE_LEMME  | AFFICHAGE_SURFACE | AFFICHAGE_CATEGORIE | AFFICHAGE_ARBRE_CHUNK;
            continue;
        }
        
        if (!strcmp(argv[i],"-tr")) {
            laffichage=laffichage & ~tousaffichages;
            laffichage|=AFFICHAGE_SURFACE | AFFICHAGE_REDUIT | AFFICHAGE_ARBRE_CHUNK;
            continue;
        }
        if (!strcmp(argv[i],"-lem")) {
            laffichage=AFFICHAGE_LEMME | AFFICHAGE_REDUIT | AFFICHAGE_ARBRE_CHUNK;
            continue;
        }

		if (!strcmp(argv[i],"-mwdisplay")) {
            AFFICHE_PHRASE_MULTIMOTS=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-nouvel")) {
            nouvel_algo=1-nouvel_algo;
            continue;
        }

		if (!strcmp(argv[i],"-testutf8")) {
            laffichage|=TEST_INPUT_UTF8;
            continue;
        }
        
        if (!strcmp(argv[i],"-depfeatval")) {
            laffichage|=AFFICHAGE_TRAIT_VALEUR_DEPENDANCE;
            continue;
        }

         
        if (!strcmp(argv[i],"-markup")) {
            laffichage|=AFFICHAGE_BALISE;
            continue;
        }

        if (!strcmp(argv[i],"-cat")) {
            laffichage|=AFFICHAGE_CATEGORIE;
            continue;
        }

		if (!strcmp(argv[i],"-fullcat")) {
            laffichage|=AFFICHAGE_CATEGORIE_GENERATION;
            continue;
        }

		if (!strcmp(argv[i],"-gencat")) {
            laffichage|=AFFICHAGE_CATEGORIE_GENERATION;
			laffichage&=~AFFICHAGE_ARBRE_CHUNK;
            continue;
        }

		if (!strcmp(argv[i],"-gen")) {
            laffichage|=AFFICHAGE_GENERATION;
            continue;
        }

		/** Start MCR added code for passage XML output **/
		if (!strcmp(argv[i],"-passageXml")) {
            laffichage|=AFFICHAGE_PASSAGE_XML_FORMAT;
			// in case -xml was ommited, we define necessary variable to output in xml
			outputXML=OUI;
			continue;
        }
		/** End MCR added code for passage XML output **/

//----------------------------------------------------------
        if (!strcmp(argv[i],"-xml")) {
            outputXML=OUI;
            continue;
        }

//Affichage d'arbre
        if (!strcmp(argv[i],"-t")) {
            de_tree=ARBRE_AFFICHE;
            continue;
        }
        
        if (!strcmp(argv[i],"-tc")) {
            de_tree=ARBRE_SYNTAGME;
            continue;
        }

        if (!strcmp(argv[i],"-pythondebug")) {
            if ((i+2)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give first the module number then the line number"<<Endl;                
                return -1;
            }
            else {                
                num_fichier_python=atoi(argv[++i]);
				num_python_ligne=atoi(argv[++i]);
				python_debug=OUI;
            }
            continue;
        }

        if (!strcmp(argv[i],"-tl")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a numeral"<<Endl;                
                return -1;
            }
            else {
                de_tree=ARBRE_COLONNE;
                column=atoi(argv[++i]);
            }
            continue;
        }
//-----------------------------------------------
        if (!strcmp(argv[i],"-help")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a numeral"<<Endl;                
                return -1;
            }
			else 
                AfficheAide(atoi(argv[++i]));
            exit(-1);
        }

        if (!strcmp(argv[i],"-seek")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please give a numeral"<<Endl;                
                return -1;
            }
            else
                seek_value=atoi(argv[++i]);
            continue;
        }
        
        if (!strcmp(argv[i],"-f")) {
            fonction_synt=OUI;
            continue;
        }
        if (!strcmp(argv[i],"-trace")) {
            theTrace+=1;
            continue;
        }
        if (!strcmp(argv[i],"-dependency")) {
            traceDependance=OUI;
            continue;
        }
        if (!strcmp(argv[i],"-rules")) {
            theRules|=1;
            continue;
        }

        if (!strcmp(argv[i],"-automaton")) {
            afficheautomate=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-indent")) { 
            theTrace+=10;
            continue;
        }                                                
        if (!strcmp(argv[i],"-nprep")) { 
            continue;
        }
        if (!strcmp(argv[i],"-prep")) { 
            continue;
        }        
        if (!strcmp(argv[i],"-addhmm")) { 
            pretraitement=OUI;
            continue;
        }        
        if (!strcmp(argv[i],"-ne")) {
            //aff_fonc= AFFICHE_FONCTION|AFFICHE_NUMERO;
            affichage_offset|=AFFICHAGE_NUMERO_DEPENDANCE;
            continue;
        }
        if (!strcmp(argv[i],"-insxml")) {
            outputXML=OUTPUTLIBXML;
            affichage_offset|=AFFICHAGE_OFFSET;
            continue;
        }
        
        if (!strcmp(argv[i],"-outxml")) {
            outputXML=OUTPUTLIBXML;
            affichage_offset|=AFFICHAGE_OFFSET;
            laffichage|=AFFICHAGE_FICHIER_XML_ECRAN;
            continue;
        }
        
		if (!strcmp(argv[i],"-displaycode")) {
			affichelescodes=OUI;
			continue;
		}


        if (!strcmp(argv[i],"-tagger")) {
            tagger=OUI;
            continue;
        }
        if (!strcmp(argv[i],"-tagging")) {
            tagging=OUI;
            continue;
        }
        if (!strcmp(argv[i],"-ntagging")) {
            tagging=NON;
            continue;
        }
        if (!strcmp(argv[i],"-warning")) {
            aff_err=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-debug")) {
            laffichage|=AFFICHAGE_ERREUR_EXECUTION;
            continue;
        }

		if (!strcmp(argv[i],"-kifdebug")) {
            debugkif=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-crypte")) {
            crypte=UTILISE;
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a directory"<<Endl;                
                return -1;
            }
            else
                strcpy(crypte_repertoire,argv[++i]);

            if (crypte_repertoire[strlen(crypte_repertoire)-1]!=SEP) {
                crypte_repertoire[strlen(crypte_repertoire)]=SEP;
                crypte_repertoire[strlen(crypte_repertoire)+1]=0;
            }

            continue;
        }   
		if (!strcmp(argv[i],"-crypteduplique")) {
            crypte=UTILISE;
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a directory"<<Endl;                
                return -1;
            }
            else
                strcpy(crypte_repertoire,argv[++i]);

            if (crypte_repertoire[strlen(crypte_repertoire)-1]==SEP)
                crypte_repertoire[strlen(crypte_repertoire)]=0;
			duplication_crypte=OUI;
            continue;
        }

		if (!strcmp(argv[i],"-crypteregles")) {
            crypte=UTILISE;
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a directory"<<Endl;                
                return -1;
            }
            else
                strcpy(crypte_repertoire,argv[++i]);

            if (crypte_repertoire[strlen(crypte_repertoire)-1]==SEP)
                crypte_repertoire[strlen(crypte_repertoire)]=0;
			duplication_crypte=OUI;
			cryptejustelesregles=OUI;
            continue;
        }

		if (!strcmp(argv[i],"-isolegrammaire")) {
            crypte=UTILISE;
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<Endl<<"Please choose a directory"<<Endl;                
                return -1;
            }
            else
                strcpy(crypte_repertoire,argv[++i]);

            if (crypte_repertoire[strlen(crypte_repertoire)-1]==SEP)
                crypte_repertoire[strlen(crypte_repertoire)]=0;
			duplication_crypte=EXTRAIRE;
            continue;
        }

        if (!strcmp(argv[i],"-np")) {
            aff_fonc= AFFICHE_NUM_PHRASE;
            continue;
        } 

        if (!strcmp(argv[i],"-npi")) {
            aff_fonc=AFFICHE_NUM_PHRASE|AFFICHE_INVERSE;
            continue;
        }

        if (!strcmp(argv[i],"-feat")) {
            aff_fonc=aff_fonc|AFFICHE_TRAITS;
            continue;
        }

        if (!strcmp(argv[i],"-disj")) {
            aff_fonc=aff_fonc | AFFICHE_INCOMPATIBLE;
            continue;
        }

        if (!strcmp(argv[i],"-english")) {
            langue=ANGLAIS;
            continue;
        }

        if (!strcmp(argv[i],"-french")) {
            langue=FRANCAIS;
            continue;
        }
        
        if (!strcmp(argv[i],"-features")) {
            tracetraits=OUI;
            continue;
        }
        
        Usage();
        *flot_erreur<<Endl<<"Unknown command:"<<argv[i]<<Endl;        
        exit(-1);
    }
    
	//Compilation d'automate
	if (b_intrans || b_outtrans) {
		if (b_intrans==false || b_outtrans==false) {
			*flot_erreur<<Endl<<"Automaton compiling requires an '-intrans file -outtrans file'"<<Endl;
			exit(-1);
		}
		time(&tempsinitial);
		try {
			compileautomaton(intrans,outtrans,nbarcs,OUI);
		}
		catch(XIPError* msg) {
			*flot_erreur<<msg->message<<Endl;
			exit(-1);
		}

		time(&tempsfinal);
        duree = difftime( tempsfinal,tempsinitial);
        cout<<"Duration : "<<duree<<"s"<<Endl;
		exit(0);
	}

    //Dans le cas d'une creation d'index
    if (fichiersem.dernier>0) {
        if (fichierindex[0]==0) {
            *flot_erreur<<Endl<<"Please give the filename of your index... (-index filename)";
            exit(-1);
        }
        indexeur ind(fichierindex);
        ind.extrait_index(fichiersem);
        exit(-1);
    }
    char buf[1000];
    
    if (corpus[0]) {
        ifstream ftextes(corpus,modeOuverture);
        if (ftextes.fail()) {
            *flot_erreur<<Endl<<"Unknown corpus file"<<Endl;
            exit(-1);
        }
        
        while (!ftextes.eof()) {            
            ftextes.getline(buf,999);
            for (int bi=strlen(buf)-1;bi>0;bi--) {
                if (buf[bi]<=32)
                    buf[bi]=0;
                else
                    break;
            }
			if (strlen(buf)>3)
                textes.push_back(DupliqueChaine(buf));
        }
    }

    if (corpus_xmltexts[0]) {
        ifstream fxmltextes(corpus_xmltexts,modeOuverture);
        if (fxmltextes.fail()) {
            *flot_erreur<<Endl<<"Unknown corpus file"<<Endl;
            exit(-1);
        }
        
        while (!fxmltextes.eof()) {            
            fxmltextes.getline(buf,255);
            for (int bi=strlen(buf)-1;bi>0;bi--) {
                if (buf[bi]<=32)
                    buf[bi]=0;
                else
                    break;
            }
            if (strlen(buf)>3) {
                racinebalise.ajoute(DupliqueChaine("r"));
                encodages.ajoute(DupliqueChaine("e"));
                xmlfileinput.ajoute(OUI);
                profxmltextes.ajoute(prof_xmltexts);                
                textes.push_back(DupliqueChaine(buf));
            }
        }
    }
    
	GlobalParseur* parseur;
	if (fichierkifs.size()!=0) {
		//On initialise une valeur par defaut du declencheur 
		//de garbage, pour que celui-ci puisse s'effectuer
		//a chaque fin d'execution
		parseur=new GlobalParseur("","");		
        parseur->InitialisationParseurMin(aff_fonc,outputXML,tagger,tagging,math,aff_err,afficherResultats,
            ordreDependance,affichage_offset,laffichage,de_tree,fonction_synt);

		parseur->iParseurCourant=0;
		gVecteurParseurs.ajoute(parseur);
		parseur->kifcode=new KifCode(parseur,kif_declencheur);
		parseur->kifcode->SetArguments(kifarguments);
		for (i=0;i<fichierkifs.size();i++) {
			parseur->NOM=STR(fichierkifs[i]);
			ifstream fkif(STR(fichierkifs[i]),modeOuverture);
			if (fkif.fail() ) {
				cerr<<"Cannot open kif file: "<<parseur->NOM<<endl;
				exit(-1);
			}
			try {
				parseur->LIGNES=1;				
				cerr<<"Loading kif file: "<<parseur->NOM<<endl;
				parseur->kifcode->Loadfile(fkif,fichierkifs[i],0);
			}
			catch(KifRaiseError* m) {
				cerr<<STR(m->message)<<" at "<<m->left<<endl;
				delete m;
				delete parseur;
				exit(-1);
			}	
		}
		if (debugkif==OUI)
			parseur->kifcode->Setdebug(true);
		parseur->kifcode->Run();
		if (parseur->kifcode->error==true)
			cout<<STR(parseur->kifcode->errorptr->String())<<endl;
		delete parseur;
		exit(0);
	}
    
    if (lePort==0 && crypte!=UTILISE && !arret && i == argc && !fromPipe && textes.dernier==0 && fusiongrm==NON) {
        Usage();
        *flot_erreur<<Endl<<"please enter the name of the file to process..."<<Endl;        
        exit(-1);
    }
    else data_pos=i;
    
    char cryptation;

    try {
        if (grmfile.dernier!=0) {
            cryptation=LoadGrmFiles(grmfile[0],
                ntmfile,
                hmmfile,
                grammaire,
				generefile,
                tokenfile,
                lookupfile,
                strategy,
                lookup_flags,
                conversion,
                fileparaminter,
                g_definition,
				blockingrule,
				OUI);
            for (fili=0;fili<fileparam.dernier;fili++)
                fileparaminter.ajoute(fileparam[fili]);
            fileparam.raz();
            fileparam.affecte(fileparaminter);
            fileparaminter.raz();
        }
    }
    catch(XIPError* msg) {
       *flot_erreur<<msg->message<<Endl;
       exit(-1);
    }
    
    if (crypte!=UTILISE)
        crypte=cryptation;

    if (lePort==0 && !p_fichier && ntmfile[0]==0) {
        Usage();
        *flot_erreur<<Endl<<"Please enter the filename which contains the parameters of the parser"<<Endl;        
        exit(-1);
    }
    
    char* last_slash= strrchr(grammaire,SEP);
    
    if (last_slash==NULL)
#ifdef WIN32
        strcpy(path,"");
#else
    strcpy(path,".");
#endif
    else {
        *last_slash=0;
        strcpy(path,grammaire);
        strcpy(grammaire,last_slash+1);
    }
    
    int iParseur;
    GlobalParseur* leParseur=NULL;

#ifndef _DEBUG    
    try {
#endif        
        
        
        leParseur=ChoixParseur(inputXML,grammaire,generefile,ntmfile,hmmfile,tokenfile,lookupfile,path,strategy,lookup_flags);
        for (ide=0;ide<g_definition.size();ide++)
            leParseur->definition_var.push_back(g_definition[ide]);

		leParseur->num_fichier_python=num_fichier_python;
		leParseur->num_python_ligne=num_python_ligne;
		leParseur->python_debug=python_debug;


        if (crypte==NON)
            leParseur->TRACEREGLE=theRules;
/*

 Parameters:
    leParseur = the pointer on the current parsing variable (initialized above)        
    langue= the language variable that helps to choose between English and French 
            for the keywords and error messages.
         
    path = the path of the parameter file that is extracted from the parameter grammaire itself
    grammaire = the filename of the parameter file
    numberValues = maximum number of terminal feature values (overide field number in parameter file)
    default value is -1 (does not overide field number)
    theTrace = 0 no trace
             1 trace file produced
             10 indented file produced
             11 trace file and indented file produced
    aff_fonc= 0 no number displayed
              1 display the extraction and deduction rule numbers
              2 add the phrase number in the dependencies, do not display the chunk tree)
              3 same as above but also display the inverted form of the dependencies
              4 xml output
    tagger = display result as a tagger
    tagging = uses tagging rules in the parsing
    crypte = data files are crypted
    aff_err = display warning messages on unknown words and tags (from an external lexicon)
    afficherResultats = only execution, no output
    offset = display the offset numbers of each word (need NTM)
    laffichage = 0 complete displayed of nodes
             1 partial display
             2 only surface form are displayed
    de_tree = 0 no tree is displayed, ou bien ARBRE_AFFICHE,ARBRE_SYNTAGME,ARBRE_COLONNE
    fonction_synt = corresponds to -f option. Launch the extraction of dependencies
    fileparam = list of filenames of extra parameter files.
*/
        
        leParseur->TRACEDEPENDANCE=traceDependance;
        leParseur->TRACETRAITS=tracetraits;
        
        
        
        iParseur=leParseur->InitialisationParseur(langue,
            stringfile,
            path,
            numberValues,
            theTrace,
            aff_fonc,
            outputXML,
            tagger,
            tagging,
            math,
            crypte,
            aff_err,
            afficherResultats,
            ordreDependance,
            affichage_offset,
            laffichage,
            de_tree,
            fonction_synt,
            fileparam);
        
        leParseur->ChoixUTF8(conversionutf8);
        leParseur->GenereOptimise(optimize);
        if (XipSetLabelRuleLimit(iParseur,label_limite)==NON) {
            *flot_erreur<<"Unknown label: "<<label_limite<<Endl;
            exit(-1);
        }
		if (modeOutC==OUI)			
			leParseur->OS=new ostringstream;

		for (ide=0;ide<blockingrule.size();ide++)
			blockingrulespaces.ajoute((char*)blockingrule[ide].c_str());
        
		for (i=0;i<blockingrulespaces.dernier;i++) {
			if (leParseur->desactiveEspaceRegle(blockingrulespaces[i])==NON) {
				cerr<<"Unknown rulespace:"<<blockingrulespaces[i]<<Endl;
				exit(-1);
			}
		}

		if (probafile!="")
			leParseur->chargeprobabilites(probafile);

#ifdef XIPLIBXML
        XipSetResultDirectory(iParseur,repertoireresultat);
        //Lecture des fichiers XML
        for (i=0;i<filexml.dernier;i++)        
            leParseur->ChargerFichierXML(filexml[i],alias[i]);
        if (xtdfile[0])
            leParseur->AnalyseXDTD(xtdfile);
#endif
        
        //Chargement des autres langues:
        for (int iautreparseur=1;iautreparseur<grmfile.dernier;iautreparseur++) {
            ntmfile[0]=0;
            hmmfile[0]=0;
            grammaire[0]=0;
            g_definition.clear();
			blockingrule.clear();
            lookup_flags[0]=0;
			tokenfile[0]=0;
			lookupfile[0]=0;
            cryptation=LoadGrmFiles(grmfile[iautreparseur],
                ntmfile,
                hmmfile,
                grammaire,
				generefile,
                tokenfile,
                lookupfile,
                strategy,
                lookup_flags,
                conversion,
                fileparaminter,
                g_definition,
				blockingrule,
				OUI);
            
            char* last_slash= strrchr(grammaire,SEP);
            
            if (last_slash==NULL)
#ifdef WIN32
                strcpy(path,"");
#else
            strcpy(path,".");
#endif
            else {
                *last_slash=0;
                strcpy(path,grammaire);
                strcpy(grammaire,last_slash+1);
            }

            GlobalParseur* autre_parseur=ChoixParseur(inputXML,grammaire,generefile,ntmfile,hmmfile,tokenfile,lookupfile,path,strategy,lookup_flags);
            if (crypte==NON)
                autre_parseur->TRACEREGLE=theRules;
            autre_parseur->TRACEDEPENDANCE=traceDependance;
            autre_parseur->TRACETRAITS=tracetraits;
			autre_parseur->num_fichier_python=num_fichier_python;
			autre_parseur->num_python_ligne=num_python_ligne;
			autre_parseur->python_debug=python_debug;

            for (ide=0;ide<g_definition.size();ide++)
                autre_parseur->definition_var.push_back(g_definition[ide]);

            
            autre_parseur->InitialisationParseur(langue,
                stringfile,
                path,
                numberValues,
                theTrace,
                aff_fonc,
                outputXML,
                tagger,
                tagging,
                math,
                crypte,
                aff_err,
                afficherResultats,
                ordreDependance,
                affichage_offset,
                laffichage,
                de_tree,
                fonction_synt,
                fileparaminter);
            
            //On garde la trace de ces fichiers
            for (fili=0;fili<fileparaminter.dernier;fili++)
                TraceFichiersParam.ajoute(fileparaminter[fili]);
            fileparaminter.raz();
            autre_parseur->ChoixUTF8(conversionutf8);
            autre_parseur->GenereOptimise(optimize);
			if (fusiongrm==OUI) {
				comparegrammaire(leParseur,autre_parseur,crypte_repertoire,grmfile[0],grmfile[1]);
				exit(0);
			}
        }

#ifndef _DEBUG
    }
    catch(...) {
        *flot_erreur<<"InitParseur error"<<Endl;
        exit(-1);
    }
#endif
	if (fusiongrm==OUI) {
		Usage();
		*flot_erreur<<Endl<<"Please provide the two grammars to differentiate"<<Endl;        
        exit(-1);
	}

    if (crypte==UTILISE)
        exit(0);

    if (crypte==NON) {
        *flot_erreur<<"Rules"<<Endl<<Endl;
        *flot_erreur<<"Nb Features:\t\t"<<leParseur->iNombreDeValeurs<<Endl;
        *flot_erreur<<"Number of Layers:\t"<<leParseur->MaxProfondeurAnalyse<<Endl;
        *flot_erreur<<"Disambiguation Rules:\t"<<leParseur->nbSimplifie<<Endl;
        *flot_erreur<<"Sequence Rules:\t\t"<<leParseur->nbSequence<<Endl;
        *flot_erreur<<"ID-rules:\t\t"<<leParseur->nbDI<<Endl;
        *flot_erreur<<"Marking Rules:\t\t"<<leParseur->nbMarqueur<<Endl;
        *flot_erreur<<"Inference Rules:\t"<<leParseur->nbInference<<Endl;
        *flot_erreur<<"Number of Lexemes:\t"<<leParseur->numero_vocabulaire<<Endl;
        *flot_erreur<<"Lexical Rules:\t\t"<<leParseur->numero_regle_semantique<<Endl;
        *flot_erreur<<"Factorized Features:\t"<<leParseur->tailleFactorise<<" out of "<<leParseur->appelFactorise<<Endl<<Endl;
    }
    
    
    if (calcultemps!=NON) {
        time(&tempsinitial);
        time(&tempsbasephrase);        
    }
    
    if (genere==OUI)
        leParseur->CalculFonction=2;
    
    if (pretraitement==OUI)
        leParseur->Pretraitement(pretraitement);
    
    parseur_courant=leParseur;
    leParseur->socketcourante=-1;
    leParseur->MaxSentence=maxSentence;
    leParseur->prolog=mode_prolog;
    int itest;
    for (itest=0;itest<9;itest++)
        test[itest]=0;

    FILE* document;
    
    leParseur->colonne=column;
	int nombre_de_phrase=0;
    
	//ofstream osxml("c:\\XIP\\test\\sorties\\out.txt");
	//XipParseFileXMLToFile(textes[0],0,&osxml,0,XML_NONE,1);

    if (lePort!=0) {
        if (leParseur->ServeurLicense==NON) {
            *flot_erreur<<"This license does not permit the utilization of XIP as a server"<<Endl;
            *flot_erreur<<"Please contact Xerox for a modification of your license agreement..."<<Endl;
            exit(-1);
        }
        leParseur->lePortServeur=lePort;
        leParseur->LancerServeur();
    }
    else {        
        leParseur->appliqueGrammaire=analyse;
        leParseur->appliqueGrammaireDefini=leParseur->appliqueGrammaire;

		//char* dupl=NULL;
		//char* en=strdup("Toto\ttiti\t+Noun+Fem+TOK+ti+ADJ+Fem+TOK+i+ADJ+Masc");
		//leParseur->LexiconEntryParsing(en,&dupl);
		//leParseur->ConstruitForme(en,0);
        //ostringstream oss;
        //XipParseStringOS("The lady speaks",0,&oss,1);
        //cout<<oss.str()<<Endl;

		unsigned char lec;
        if (textes.dernier==0) {
			if (affichelescodes==OUI) {
				while (!feof(stdin)) {
					lec=fgetc(stdin);
					if (lec<128)
						cout<<lec;
					else
						cout<<"#"<<(int)lec<<" ";
				}
				exit(0);
			}
            leParseur->FileAssociation(stdin);
            leParseur->FileStreamAssociation(&cin);
            leParseur->Analyse(renumerote,tree);
        }
        else {
            ifstream* fichierdonnees=NULL;
            for (int itext=0;itext<textes.dernier;itext++) {
                *flot_erreur<<textes[itext]<<Endl;          
				string sortietexte=textes[itext];
				sortietexte+=".txt";
				ofstream* sortietexteStream=NULL;
				if (sortiepredef==OUI) {
					if (leParseur->OS!=NULL) {
						cerr<<"You cannot use both -output and -outc"<<endl;
						exit(-1);
					}
					sortietexteStream=new ofstream((char*)sortietexte.c_str());
					leParseur->OS=sortietexteStream;
				}
				if (affichelescodes==OUI) {
					document=fopen(textes[itext],"rb");
					while (!feof(document)) {
						lec=fgetc(document);
						if (lec<128)
							cout<<lec;
						else
							cout<<"#"<<(int)lec<<" ";
					}
					exit(0);
				}

				leParseur->DeductionInitiale=OUI;

#ifdef XIPLIBXML
                if (inputXML==OUI) {
                    GlobalParseurXML* xmlparse=(GlobalParseurXML*)leParseur;
                    xmlparse->AnalyseXipXMLFilename(textes[itext]);
                    continue;
                }
                else {
                    if (xmlfileinput[itext]==OUI) {
                        //cas particulier, nous allons traiter un fichier texte dont le contenu est en XML
                        //Le fichier est analysé dans libxml, ce qui implique de ne fournir que le nom
                        //du fichier ainsi que la DTD...
                        leParseur->AnalyseFichierXML(textes[itext],profxmltextes[itext],renumerote,tree);
                    }
                    else {
                        //Autre cas particulier. On lit un fichier texte que l'on regenere sous la forme
                        //d'un fichier XML.
                        if (xmlfileinput[itext]==OUTPUTGENEREXML) 
                            leParseur->GenereFichierXML(textes[itext],
                                racinebalise[itext],
                                encodages[itext],
                                renumerote,
                                tree);
                    }
                }
#endif
				//Cas specifique pour la generation, on entre un fichier
				//dont le format est: dep(cat[feat,lemma=""],cat[feat,lemma=""])
				if (dependances[itext]==OUI) {
					fichierdonnees= new ifstream(textes[itext],modeOuverture);
					if (fichierdonnees->fail()) {
						*flot_erreur<<"Unknown text file:"<<textes[itext]<<Endl;
						continue;
					}
					leParseur->FileStreamAssociation(fichierdonnees);
					leParseur->ChargerDependances(textes[itext]);
				}
				else {
					//Modes normaux d'analyse                
					if (xmlfileinput[itext]==NON) {
						char fermedoc=NON;
						if (inputXML==NON) {
							document=fopen(textes[itext],"rb");
							if (document==NULL) {
								sprintf(err,"%s%s",leParseur->CHAINES[97],textes[itext]);
								*flot_erreur<<err<<Endl;
								continue;
							}
							leParseur->renseigne(textes[itext]);
							if (seek_value!=-1)
								fseek(document,seek_value,SEEK_SET);
							leParseur->FileAssociation(document);
							fermedoc=OUI;
						}
						else {
							fichierdonnees= new ifstream(textes[itext],modeOuverture);
							if (fichierdonnees->fail()) {
								*flot_erreur<<"Unknown text file:"<<textes[itext]<<Endl;
								continue;
							}
							leParseur->FileStreamAssociation(fichierdonnees);
						}

						leParseur->Analyse(renumerote,tree);

						if (fermedoc==OUI)
							fclose(document);
					}
				}

				if (fichierdonnees!=NULL) {
					fichierdonnees->close();
					delete fichierdonnees;
					fichierdonnees=NULL;
				}                    				
				if (sortietexteStream!=NULL) {
					sortietexteStream->close();
					delete sortietexteStream;
					leParseur->OS=NULL;
				}
				nombre_de_phrase+=leParseur->num_phrase;
            }
        }		
    }
    
    if (calcultemps!=NON) {        
        float valreu;
        time( &tempsfinal );
        duree = difftime( tempsfinal,tempsinitial);
		cerr<<"Time : "<<duree<<"s"<<Endl;
        cout<<"Duration : "<<duree<<"s"<<Endl;
        cout<<"Words : "<<leParseur->nombre_mots<<Endl;
        cout<<"Sentences : "<<nombre_de_phrase<<Endl;
        if (duree>0) {
            valreu=leParseur->nombre_mots/duree;
            cout<<"Words/s : "<<valreu<<Endl;
        }
		cout<<"Number of dependencies tested: "<<tentative_boucle_dependance<<Endl<<Endl;
        cout<<"Number of dependencies extracted or modified: "<<resolution_reussite<<Endl<<Endl;
        cout<<"Number of dependencies extracted: "<<nb_dependances<<Endl<<Endl;
        
		//initialiser avec -loopstats
        if (statistiques==OUI) {
            //for (itest=0;itest<9;itest++)
            //    cout<<"Echec("<<itest<<"):"<<test[itest]<<Endl;

            cout<<"Number of loops in apply inference:"<<nombre_boucle_inference<<Endl;
            cout<<"Number of non recursive loops in apply inference:"<<nombre_derecursivation<<Endl;
            cout<<"Tentative: "<<(int)resolution_tentative<<Endl;
            valreu=(resolution_reussite/resolution_tentative)*100;
            cout<<"Success: "<<valreu<<Endl;
            valreu=resolution_quantite/resolution_tentative;
            cout<<"Number of inference failure: "<<resolution_quantite<<Endl;
			cout<<"Number of inference test failure: "<<nb_echec_inference<<Endl;
            cout<<"Average Number of loops before failure: "<<valreu<<Endl;
            cout<<"Number of loops for a sequence rule: "<<nombre_boucle_sequence<<Endl;
            cout<<"Number of loops for a marking rule: "<<nombre_boucle_marquage<<Endl;
            cout<<"Number of call to construction:"<<nb_test_marquage<<Endl;
            cout<<"Number of loops in ConstruitIndex: "<<appel_construit_index<<Endl;
            cout<<"Number of inference loop: "<<appel_inference<<Endl;
            cout<<"Number of call to inference: "<<appel_calcul_inference<<Endl;
            valreu=((float)reussite_inference/(float)appel_calcul_inference)*100;
            cout<<"Number of success: "<<reussite_inference<<"("<<valreu<<"%)"<<Endl;
            valreu=((float)echec_a_1/(float)appel_calcul_inference)*100;
            cout<<"Echec en moyenne au bout: "<<valreu<<Endl;
            cout<<"Erreur sur noeud inference: "<<erreur_inference_noeud<<Endl;
            cout<<"Erreur sur dependance inference: "<<erreur_sur_dependance<<Endl;            
            cout<<"Nb Comparaison:"<<i_compte<<Endl;
            cout<<"AppliqueInference time:"<<(long)dureeaccumulee<<Endl;
            cout<<"Appel AppliqueInference:"<<appelinf<<Endl;
            cout<<"Moyenne:"<<(long)moyenneapplique<<Endl;
            
        }
    }
    
    for (i=0;i<fileparam.dernier;i++)
        libere(fileparam[i]);
    
    for (i=0;i<filexml.dernier;i++) {
        libere(filexml[i]);
        libere(alias[i]);
    }

    for (i=0;i<fileparam.dernier;i++)
        libere(TraceFichiersParam[i]);
   
    for (i=0;i<textes.dernier;i++)
        libere(textes[i]);

    for (i=0;i<grmfile.dernier;i++)
        libere(grmfile[i]);


    for (i=0;i<racinebalise.dernier;i++) {
        libere(racinebalise[i]);
        libere(encodages[i]);
    }

	if (modeOutC==OUI)
		delete leParseur->OS;
	else
		cout.flush();

 //   XipFreeAllGrammars();    
 //   DetruitEcran();
	//KifFinalClean();
	return 0; 
}
#endif

