#if defined(WIN32) & !defined(_WINDLL)
extern char AFFICHAGEDOS;
#endif

#include "parsbin.h"
#include "derivation.h"

void AfficheAide(int h);
char XipSetLabelRuleLimit(int ipar,string num_rule);
extern ostream* flot_erreur;


extern int VERIF;
extern float resolution_reussite;
extern char modeOutC;
extern char calcultemps;
extern time_t   tempsbasephrase;
extern char affichage_supplementaire;
extern int appel_inference;
extern int appel_calcul_inference;
extern int reussite_inference;
extern int nb_dependances;
extern char affiche_regle_appliquee;
extern char afficheautomate;
extern char python_debug;
extern int num_python_ligne;
extern int num_fichier_python;

extern char statistique_interne;
void XipFreeAllGrammars();

char CurrentLoadGrmFiles(char* grmname,
             char*  ntm,
             char*  hmm,
             char*  grammar, 
             char* tokenize,
             char*  lookup,
             lookup_strategy& strategy,
             char* lookup_flags,
             char& conversion,
             VECTA<char*>& parameters,
             vector<string>& g_definition_var);

void Usage()
{
    *flot_erreur<<"Usage:" <<endl;
	*flot_erreur<<"     -help chapter (prints out some information about XIP instruction set. -1 displays all)"<<endl;
    *flot_erreur<<"     -grm file (load a grammar file)"<<endl;
    *flot_erreur<<"     -english (comments and commands are in English"<<endl;
    *flot_erreur<<"     -french (comments and commands are in French"<<endl;
    *flot_erreur<<"     -string filename (personal string file)"<<endl;
    *flot_erreur<<"     -text filename (text to parse)"<<endl;
    *flot_erreur<<"     -texts filename (a file that comprises on each line a filename to parse)"<<endl;
    *flot_erreur<<"     -seek value (starts the analysis of a text file at value)"<<endl;
    *flot_erreur<<"     -ntm ntmscript"<<endl;
    *flot_erreur<<"     -ntmonly"<<endl;
    *flot_erreur<<"     -pathnode (displays the node with their complete path)"<<endl;
    *flot_erreur<<"     -hmm hmmfilename"<<endl;
    *flot_erreur<<"     -time (computes time of analysis)"<<endl;
    *flot_erreur<<"     -number value (overides field number in parameter file)"<<endl;
    *flot_erreur<<"     -g (parameter file for a given language)"<<endl;
    *flot_erreur<<"     -tagger (tagging mode, no parsing)"<<endl;
    *flot_erreur<<"     -tagging (disambiguation rules are used while parsing)"<<endl;
    *flot_erreur<<"     -ntagging (disambiguation rules are not used while parsing)"<<endl;
    *flot_erreur<<"     -a (for a full display of syntactic node information)"<<endl;
    *flot_erreur<<"     -r (for a reduced display of syntactic node information)"<<endl;
    *flot_erreur<<"     -tr (for an even more reduced display of syntactic node information)"<<endl;
    *flot_erreur<<"     -lem (same as -tr but with lemmas instead of surface form)"<<endl;
    *flot_erreur<<"     -cat (displays the leaf category. Sould be used AFTER -lem or -tr)"<<endl;
    *flot_erreur<<"     -disj (displays the disjunctive sets for incompatible dependencies)"<<endl;
    *flot_erreur<<"     -nm (displays the word number)"<<endl;
    *flot_erreur<<"     -obn (order by node, the dependencies are displayed according to their first parameter)"<<endl;
    *flot_erreur<<"     -ord (dependencies are ordered according to their creation)"<<endl;
    *flot_erreur<<"     -depfeatval (displays the value of the feature dependencies)"<<endl;
    *flot_erreur<<"     -t (display of the syntactic tree)"<<endl;
    *flot_erreur<<"     -tl number (automatically wraps the syntactic tree at 'number')"<<endl;
    *flot_erreur<<"     -tc (displays of the syntactic tree. Each sub-node under the top node is displayed independently)"<<endl;
    *flot_erreur<<"     -offset (display offset values for each token)"<<endl;
    *flot_erreur<<"     -sentence (display the sentence that has been analyzed)"<<endl;
    *flot_erreur<<"     -sentencenum (work with -sentence, display the sentence number with the sentence itself)"<<endl;
    *flot_erreur<<"     -ntree (the chunk tree is not displayed)"<<endl;
    *flot_erreur<<"     -prolog (dependencies are displayed in prolog form)"<<endl;
    *flot_erreur<<"     -inpututf8 (Uses this flag if the text is in UTF-8 format)"<<endl;
    *flot_erreur<<"     -outpututf8 (Uses this flag to output the text in UTF-8 format)"<<endl;    
    *flot_erreur<<"     -tokenize (Each line in the input file is a single token only valid with a NTM script)"<<endl;    
    *flot_erreur<<"     -languagechecking (The parser checks for each sentence whether it belongs to the current grammar language)"<<endl;
    *flot_erreur<<"     -no_normalization (Does not normalize the translation tags to lowercase)"<<endl;
#if defined(WIN32)
    *flot_erreur<<"     -dos (Translates characters in DOS format)"<<endl;
#endif
    *flot_erreur<<"     -f (extraction of the dependencies)"<<endl;
    *flot_erreur<<"     -nodisplay (only execution, no output)"<<endl;
    *flot_erreur<<"     -max nb (maximum number of sentences that will be analyzed"<<endl;
    *flot_erreur<<"     -sent (only displays the sentences with their number as they will be processed"<<endl;
    *flot_erreur<<"     -trace (generates a trace in the file defined in the configuration file)"<<endl;
    *flot_erreur<<"     -dependency (generates a trace fils only for the dependency calculus)"<<endl;
    *flot_erreur<<"     -rules (generates a trace file containing rules and their actual index)"<<endl;
    *flot_erreur<<"     -automaton (generates a trace file with the rule automata inside)"<<endl;
    *flot_erreur<<"     -indent (generates a file containing the result of the parse as indented trees)"<<endl;
    *flot_erreur<<"     -ne  (displays a dependency with the list of rules that applied to create it)"<<endl;
    *flot_erreur<<"     -np (adds the phrase number in the dependency output, does not display the chunk tree)"<<endl;
    *flot_erreur<<"     -npi (same as -np but also displies an inverted form of the dependencies)"<<endl;
    *flot_erreur<<"     -feat (displays the feature structure for each token in tagger mode)"<<endl;
    *flot_erreur<<"     -addhmm (adds the hmm features for the non-selected readings after an HMM tagging)"<<endl;
    *flot_erreur<<"     -math (displays or transfers the result of mathematical formulae)"<<endl;
    *flot_erreur<<"     -mathonly (only display the result of mathematical formulae, it does not display other results)"<<endl;
    *flot_erreur<<"     -upto  label|num (run the grammar up to a certain label or a rule number)"<<endl;
    *flot_erreur<<"     -optimize file (generate a file with the optimized dependency rules)"<<endl;
    *flot_erreur<<"     -xml (the output is displayed as an xml output)"<<endl;
#ifdef XIPLIBXML
    *flot_erreur<<"     -xmldb alias filename"<<endl;
    *flot_erreur<<"\t\t   alias is the name through which the database is accessed in rules"<<endl;
    *flot_erreur<<"\t\t   filename the filename of the database"<<endl;
    *flot_erreur<<"     -xmltext depth filename (an xml file which will be parsed according to the XMLGUIDE provided by the grammar)"<<endl;
    *flot_erreur<<"     -xmltexts depth filename (a text file which contains xml filenames which will be parsed according to the XMLGUIDE provided by the grammar)"<<endl;
    *flot_erreur<<"\t\t   depth is used to split an XML document into chunks according to their depth,"<<endl;
    *flot_erreur<<"\t\t   it is especially useful to treat large XML document that could not be loaded at once in memory."<<endl;
    *flot_erreur<<"     -insxml inserts the result of the analysis into the xml file as output"<<endl;
    *flot_erreur<<"     -outxml inserts the result of the analysis into the xml file as output, the result is then displayed on screen"<<endl;
    *flot_erreur<<"     -txt2xml root encoding filename (creates a new xml file out of a text file with 'root' as the root markup tag and with a certain encoding)"<<endl;
    *flot_erreur<<"     -resultxmldir path (the path where result files are stored)"<<endl;
#endif
    *flot_erreur<<"     -markup (the content of the XML tags being analyzed is displayed on screen)"<<endl;
    *flot_erreur<<"     -warning (displays the unknown tag warnings (lexicon tags that have not been associated with a parser category)"<<endl;
    *flot_erreur<<"     -debug (displays the execution error of XIP instructions)."<<endl;
    *flot_erreur<<"     -sem filename (this command is used to create an index file)"<<endl;
    *flot_erreur<<"\t\t   The filename must contain semantic disambiguation rules."<<endl;
    *flot_erreur<<"\t\t   More than one file can be indexed in one step."<<endl;
    *flot_erreur<<"\t\t   The -index must be provided on the command line."<<endl;
    *flot_erreur<<"     -index filename (this command must be used together with -sem. The filename stores the index computed out of the -sem filenames)"<<endl;
    *flot_erreur<<"     -p filename (filename comprises new rules and definition to add to the kernel grammar)"<<endl;
    *flot_erreur<<"     -renum (for each new sentence, word number starts at 0)"<<endl;
    *flot_erreur<<"     -nrenum (the first word number of a new sentence continues those of the previous sentences)"<<endl;    
    *flot_erreur<<endl<<"     \"a sentence\" or < file"<<endl;
}

void affiche_version() {
        string question;

        Whoami(question);
    *flot_erreur<<endl<<"\t"<<ConversionChaine(NULL,"Copyright © Xerox Corporation 2000-2005")<<endl
        <<"\t"<<question<<endl
        <<"\tXerox Research Center Europe"<<endl
#ifdef NTMCONNECTION
        <<"\t(TOKENIZER, LOOKUP, NTM and HMM included)"<<endl;
#else
    <<"\t(TOKENIZER, LOOKUP, NTM and HMM NOT included)"<<endl;
#endif
#ifdef XIPPYTHON
    *flot_erreur<<"\tPython enable"<<endl;
#endif
#ifdef XIPLIBXML
    *flot_erreur<<"\tXML enable"<<endl;
#endif
    *flot_erreur<<endl<<"\tImplementation:"<<endl
        <<"\t\tClaude Roux"<<endl
        <<endl<<"\tConception:"<<endl
        <<"\t\t"<<ConversionChaine(NULL,"Salah Aït-Mokhtar")<<endl
        <<"\t\tCaroline Brun"<<endl
        <<"\t\tJean-Pierre Chanod"<<endl;
    *flot_erreur<<"\t\tCaroline "<<ConversionChaine(NULL,"Hagège")<<endl
        <<"\t\tAaron Kaplan"<<endl
        <<"\t\tClaude Roux"<<endl
        <<"\t\tGilbert Rondeau"<<endl;
    *flot_erreur<<"\t\t"<<ConversionChaine(NULL,"Frédérique Segond")<<endl;
    *flot_erreur<<"\t\t"<<ConversionChaine(NULL,"Ágnes Sándor")<<endl<<endl;
}


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
        *flot_erreur<<endl<<"Please enter the name of the file to process"<<endl;        
        return -1;
    }
    

    InitialisationSignaux();
    long seek_value=-1;
    u_short lePort=0;
    affiche_regle_appliquee=NON;
    lookup_strategy strategy=NO_STRATEGY;
    vector<string> g_definition;
    char grammaire[1000];
    char path[1000];
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
    int fili;
    TVecteur ordreDependance=AFFICHAGE_DEP_NOM;
    typeLangue langue=ANGLAIS;
    VECTA<char*> fileparam;
    VECTA<char*> fileparaminter;
    VECTA<char*> filexml;
    VECTA<char*> alias;
    VECTA<int> profxmltextes;    
    VECTA<char*> fichiersem;
    VECTA<int> xmlfileinput;
    VECTA<char*> encodages;
    VECTA<char*> racinebalise;
    char fichierindex[1000];
    char err[1000];
    int maxSentence=-1;
    char conversion=NON;
    char repertoireresultat[1000];
    int prof_xmltexts=0;
    char corpus_xmltexts[1000];
    long conversionutf8=0;
    int ide;

    char xtdfile[1000];
    char hmmfile[1000];
    char ntmfile[1000];
    char stringfile[1000];
    char label_limite[100];
    char lookupfile[1000];
    char tokenfile[1000];
    VECTA<char*> grmfile;
    char optimize=NON;
    VECTA<char*> textes;
    char renumerote=OUI;
    char tree=OUI;
    char analyse=OUI;
    
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
    VECTA<char*> TraceFichiersParam;
    char lookup_flags[100];
    lookup_flags[0]=0;
    
    //the language argument is processed
    int arret=0;
    int numberValues=-1;
    data_pos=1;
    char tracetraits=NON;
    
    char corpus[1000];
    
    corpus[0]=0;
    
    calcultemps=NON;
    time_t   tempsinitial,tempsfinal;
    double   duree;
    char math=NON;
    
    
    VERIF=NON;
    
    
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
    

    for (i = 1; i <argc ;i++) {
        
        if (!strcmp(argv[i],"-l") || !strcmp(argv[i],"-g")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else {
                strcpy(grammaire,argv[++i]);
                p_fichier=1;
            }
            continue;
		}
        
        if (!strcmp(argv[i],"-index")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else 
                strcpy(fichierindex,argv[++i]);
            continue;
        }
        
        

        if (!strcmp(argv[i],"-grm")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else {
                grmfile.ajoute(DupliqueChaine(argv[++i]));
                p_fichier=1;
            }
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

        if (!strcmp(argv[i],"-tokenize")) {
            laffichage|=EXECUTE_TOKENIZE;
            continue;
        }
        
#ifdef XIPLIBXML       
        if (!strcmp(argv[i],"-resultxmldir")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
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
                *flot_erreur<<endl<<"Missing arguments for -xmldb"<<endl;                
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
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else                
                strcpy(xtdfile,argv[++i]);                
            continue;
        }

        if (!strcmp(argv[i],"-upto")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please insert a label or a rule number"<<endl;                
                return -1;
            }
            else                
                strcpy(label_limite,argv[++i]);                
            continue;
        }

        if (!strcmp(argv[i],"-xmltext")) {

            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else {
                
                int prof=0;         
                charg=argv[++i];
                if (charg[0] >='0' && charg[0]<='9' && charg[1]==0) {
                    prof=atoi(charg);
                    if ((i+1)>=argc) {
                        Usage();
                        *flot_erreur<<endl<<"Please choose a filename"<<endl;                        
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
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else {
                
                int prof=0;         
                charg=argv[++i];
                if (charg[0] >='0' && charg[0]<='9' && charg[1]==0) {
                    prof_xmltexts=atoi(charg);
                    if ((i+1)>=argc) {
                        Usage();
                        *flot_erreur<<endl<<"Please choose a filename"<<endl;                        
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
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else
                fileparam.ajoute(DupliqueChaine(argv[++i]));
            continue;
        }
        
        if (!strcmp(argv[i],"-sem")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else
                fichiersem.ajoute(DupliqueChaine(argv[++i]));
            continue;
        }
        
        if (!strcmp(argv[i],"-string")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else {
                strcpy(stringfile,argv[++i]);
                langue=PERSONNEL;
            }
            continue;
        }
        
        if (!strcmp(argv[i],"-hmm")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
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
        
        if (!strcmp(argv[i],"-text")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
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
        
        if (!strcmp(argv[i],"-texts")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else
                strcpy(corpus,argv[++i]);
            continue;
        }

        if (!strcmp(argv[i],"-text2xml")) {
            if ((i+3)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Missing arguments"<<endl;                
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
                *flot_erreur<<endl<<"Please choose a filename"<<endl;                
                return -1;
            }
            else
                strcpy(ntmfile,argv[++i]);
            continue;
        }
        
        if (!strcmp(argv[i],"-number")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please give a numeral"<<endl;                
                return -1;
            }
            else
                numberValues=atoi(argv[++i]);
            continue;
        }
        
        if (!strcmp(argv[i],"-max")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please give a numeral"<<endl;                
                return -1;
            }
            else
                maxSentence=atoi(argv[++i])-1;
            continue;
        }
        
        if (!strcmp(argv[i],"-port")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please give a numeral"<<endl;                
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
                *flot_erreur<<endl<<"Please give first the module number then the line number"<<endl;                
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
                *flot_erreur<<endl<<"Please give a numeral"<<endl;                
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
                *flot_erreur<<endl<<"Please give a numeral"<<endl;                
                return -1;
            }
			else 
                AfficheAide(atoi(argv[++i]));
            exit(-1);
        }

        if (!strcmp(argv[i],"-seek")) {
            if ((i+1)>=argc) {
                Usage();
                *flot_erreur<<endl<<"Please give a numeral"<<endl;                
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
        *flot_erreur<<endl<<"Unknown command:"<<argv[i]<<endl;        
        exit(-1);
    }
    
    //Dans le cas d'une creation d'index
    if (fichiersem.dernier>0) {
        if (fichierindex[0]==0) {
            *flot_erreur<<endl<<"Please give the filename of your index... (-index filename)";
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
            *flot_erreur<<endl<<"Unknown corpus file"<<endl;
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
            *flot_erreur<<endl<<"Unknown corpus file"<<endl;
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
    
    
    if (lePort==0 && !arret && i == argc && !fromPipe && textes.dernier==0) {
        Usage();
        *flot_erreur<<endl<<"please enter the name of the file to process..."<<endl;        
        exit(-1);
    }
    else data_pos=i;
    
    char cryptation=NON;

    try {
        if (grmfile.dernier!=0) {
            cryptation=CurrentLoadGrmFiles(grmfile[0],
                ntmfile,
                hmmfile,
                grammaire,
                tokenfile,
                lookupfile,
                strategy,
                lookup_flags,
                conversion,
                fileparaminter,
                g_definition);
            for (fili=0;fili<fileparam.dernier;fili++)
                fileparaminter.ajoute(fileparam[fili]);
            fileparam.raz();
            fileparam.affecte(fileparaminter);
            fileparaminter.raz();
        }
    }
    catch(XIPError* msg) {
       *flot_erreur<<msg->message<<endl;
       exit(-1);
    }
    
    char crypte=cryptation;

    if (lePort==0 && !p_fichier && ntmfile[0]==0) {
        Usage();
        *flot_erreur<<endl<<"Please enter the filename which contains the parameters of the parser"<<endl;        
        exit(-1);
    }
    
#ifdef WIN32    
    char* last_slash= strrchr(grammaire,'\\');
#else
    char* last_slash= strrchr(grammaire,'/');
#endif
    
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

#ifndef _DEBUG    
    try {
#endif        
        
        
        GlobalParseurTry* leParseur=new GlobalParseurTry(grammaire,path);
        for (ide=0;ide<g_definition.size();ide++)
            leParseur->definition_var.push_back(g_definition[ide]);

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
        if (XipSetLabelRuleLimit(iParseur,label_limite)==NON) {
            *flot_erreur<<"Unknown label: "<<label_limite<<endl;
            exit(-1);
        }
		if (modeOutC==OUI)			
			leParseur->OS=new ostringstream;


		FILE* document;
		for (int itext=0;itext<textes.dernier;itext++) {
			*flot_erreur<<textes[itext]<<endl;               
			leParseur->DeductionInitiale=OUI;
			//Modes normaux d'analyse                
			char fermedoc=NON;
			document=fopen(textes[itext],"rb");
			if (document==NULL) {
				sprintf(err,"%s%s",leParseur->CHAINES[97],textes[itext]);
				*flot_erreur<<err<<endl;
				continue;
			}
			leParseur->renseigne(textes[itext]);
			if (seek_value!=-1)
				fseek(document,seek_value,SEEK_SET);
			leParseur->FileAssociation(document);
			fermedoc=OUI;

			leParseur->Analyse(renumerote,tree);

			if (fermedoc==OUI)
				fclose(document);
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

	if (calcultemps!=NON) {
        time(&tempsinitial);
        time(&tempsbasephrase);        
    }
	
	//EXECUTION SHOULD BE HERE-----------------------------------------------------------------
    if (calcultemps!=NON) {        
        float valreu;
        time( &tempsfinal );
        duree = difftime( tempsfinal,tempsinitial);
        cout<<"Duration : "<<duree<<"s"<<endl;
        cout<<"Words : "<<leParseur->nombre_mots<<endl;
        cout<<"Sentences : "<<leParseur->num_phrase<<endl;
        if (duree>0) {
            valreu=leParseur->nombre_mots/duree;
            cout<<"Words/s : "<<valreu<<endl;
        }
        cout<<"Number of dependencies extracted or modified: "<<resolution_reussite<<endl<<endl;
        cout<<"Number of dependencies extracted: "<<nb_dependances<<endl<<endl;
	}

    XipFreeAllGrammars();    
    DetruitEcran();
    
    return 0; 
}
