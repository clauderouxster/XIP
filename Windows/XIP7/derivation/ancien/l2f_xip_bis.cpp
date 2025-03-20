// l2f_xip.cpp : Defines the entry point for the console application.

// Compilation flags: XIPDLL_EXPORT, XIPSTLAPI, XIPSTLUSE
// Libraries: libxips.lib, libpython.a

#include <iostream>
#include <string>
#include <map>

#define not !
#include "parsbin.h"
#include "derivation.h"

extern char  affichage_supplementaire;
extern char  affiche_regle_appliquee;
extern char  afficheautomate;
extern char  calcultemps;
extern char  python_debug;
extern char  statistique_interne;
extern float resolution_reussite;
extern int   nb_dependances;
extern int   num_fichier_python; 
extern int   num_python_ligne; 

void XipFreeAllGrammars();

TVecteur const Options::displaymode_complete =
    DISPLAY_FULL
    | DISPLAY_LEMMA
    | DISPLAY_SURFACE
    | DISPLAY_ENTREE
    | DISPLAY_CATEGORY
    | DISPLAY_CHUNK_TREE;

Options::Options(int const argc, char **argv)
    : analysis(OUI),
      column(80),
      convert(NON),
      compute_time(NON),
      dependcydisplay(DEPENDENCY_FUNCTION_DISPLAY),
      dependencies(NON),
      dependencyorder(DISPLAY_DEPENDENCY_BY_NAME),
      displayautom(NON),
      displayerror(NON),
      displaymode(displaymode_complete),
      displayoffset(0),
      displayother(0),
      displayresult(OUI),
      displayrules(NON),
      math(NON),
      maxSentence(-1),
      outputXML(NON),
      renumber(OUI),
      show_tree(0),
      statistics(NON),
      tagger(NON),
      tagging(OUI),
      therules(NON),
      trace_level(0),
      tracedependency(NON),
      tracefeatures(NON),
      tree(OUI),
      utf8(0),
      success(true), help_(false), std_input_(true),
      xip_spec_(0), dir(0)
{
    std::map<string, int> arguments;
    for(int i = 1; i != argc; ++i)
    arguments[std::string(argv[i])] = i;

    int number_of_processed_arguments = 0;
    
///////////////////////////////////////////////////////////////////////
// Process

    if(arguments.count("-ntmonly") == 1) {
        analysis = NON;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-max") == 1) {
        maxSentence  = atoi(strdup(argv[arguments["-max"] + 1]));
        number_of_processed_arguments += 2;
    }


///////////////////////////////////////////////////////////////////////
// HELP
    if(arguments.count("-help") == 1) {
        help_ = true;
        return;
    }


///////////////////////////////////////////////////////////////////////
// GRAMMAR FILES
    if(arguments.count("-s") == 1) {
        xip_spec_ = strdup(argv[arguments["-s"] + 1]);
        number_of_processed_arguments += 2;
    }

    if(arguments.count("-d") == 1) {
        dir = strdup(argv[arguments["-d"] + 1]);
        number_of_processed_arguments += 2;
    }

///////////////////////////////////////////////////////////////////////
// TRACE
    if(arguments.count("-trace") == 1) {
        trace_level += 1;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-indent") == 1) {
        trace_level += 10;
        ++number_of_processed_arguments;
    }
    
    if(arguments.count("-features") == 1) {
        tracefeatures = OUI;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-dependency") == 1) {
        tracedependency = OUI;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-rules") == 1) {
        therules |= 1;
        ++number_of_processed_arguments;
    }

  

///////////////////////////////////////////////////////////////////////
// TAGGER
    if(arguments.count("-tagger") == 1) {
        tagger = OUI;
        ++number_of_processed_arguments;
    }
    
///////////////////////////////////////////////////////////////////////
// DISAMBIGUATION RULES
    if(arguments.count("-tagging") == 1) {
        tagging = OUI;
        ++number_of_processed_arguments;
    }
  
    if(arguments.count("-ntagging") == 1) {
        tagging=NON;
        ++number_of_processed_arguments;
    }
 
    
///////////////////////////////////////////////////////////////////////
// DISPLAY MODE & DISPLAY OFFSET

    if(arguments.count("-a") == 1) {
        displaymode = displaymode_complete;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-tr") == 1) {
        displaymode = displaymode & ~displaymode_complete;
        displaymode |= DISPLAY_SURFACE | DISPLAY_REDUCED | DISPLAY_CHUNK_TREE;
        ++number_of_processed_arguments;
    }
    
    if(arguments.count("-ntree") == 1) {
        tree         = NON;
        displaymode &= ~DISPLAY_CHUNK_TREE;
        ++number_of_processed_arguments;
    }
    
    if(arguments.count("-r") == 1) {
        displaymode = displaymode & ~displaymode_complete;
        displaymode |= DISPLAY_FULL | DISPLAY_LEMMA | DISPLAY_SURFACE | DISPLAY_CATEGORY | DISPLAY_CHUNK_TREE;
        ++number_of_processed_arguments;
    }
    
    if(arguments.count("-lem") == 1) {
        displaymode = displaymode & ~displaymode_complete;
        displaymode |= DISPLAY_LEMMA | DISPLAY_REDUCED | DISPLAY_CHUNK_TREE;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-depfeatval") == 1) {
        displaymode |= DISPLAY_DEPENDENCY_FEATURE_VALUE;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-markup") == 1) {
        displaymode |= DISPLAY_MARKUP;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-cat") == 1) {
        displaymode |= DISPLAY_CATEGORY;
        ++number_of_processed_arguments;
    }
         
    if(arguments.count("-tokenize") == 1) {
        displaymode |= EXECUTE_TOKENIZE;
        ++number_of_processed_arguments;
    }
        
    if(arguments.count("-debug") == 1) {
        displaymode |= DISPLAY_EXECUTION_ERROR;
        ++number_of_processed_arguments;
    }
    
   if(arguments.count("-prolog") == 1) {
        displaymode &=~DISPLAY_CHUNK_TREE;
        tree         = NON;
        dependencies = OUI;
        // mode_prolog=OUI; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        ++number_of_processed_arguments;
    }
    
   if(arguments.count("-languagechecking") == 1) {
        displayoffset |= TESTER_LANGUE;
        ++number_of_processed_arguments;
    }

   if(arguments.count("-offset") == 1) {
        displayoffset |= DISPLAY_OFFSET;
        ++number_of_processed_arguments;
    }

   if(arguments.count("-sentencenum") == 1) {
        displayoffset |= DISPLAY_SENTENCE_NUMBER;
        ++number_of_processed_arguments;
    }

   if(arguments.count("-no_normalization") == 1) {
        displayoffset |= NON_NORMALISATION_TAG;
        ++number_of_processed_arguments;
    }

   if(arguments.count("-sentence") == 1) {
        displayoffset |= DISPLAY_SENTENCE;
        ++number_of_processed_arguments;
    }

   if(arguments.count("-nm") == 1) {
        displayoffset |= DISPLAY_WORDNUM;
        ++number_of_processed_arguments;
    }

   if(arguments.count("-lower") == 1) {
        displayoffset |= LOWER_INPUT;
        ++number_of_processed_arguments;
    }

   if(arguments.count("-ne") == 1) {
        displayoffset |= DISPLAY_DEPENDENCY_NUMBER;
        ++number_of_processed_arguments;
    }
    


///////////////////////////////////////////////////////////////////////
// DISPLAY OTHER & UTF8

   if(arguments.count("-pathnode") == 1) {
        displayother = 1;
        ++number_of_processed_arguments;
    }
    
    
   if(arguments.count("-outpututf8") == 1) {
        convert = OUI;
        utf8   |= DISPLAY_CONVERSION_UTF8;        
        ++number_of_processed_arguments;
    }
 
   if(arguments.count("-inpututf8") == 1) {
        convert = OUI;
        utf8   |= UTF8_INPUT;        
        ++number_of_processed_arguments;
    }



///////////////////////////////////////////////////////////////////////
// XML
    if(arguments.count("-outxml") == 1) {
        displaymode  |= DISPLAY_MERGE_XML_SUBTREE;
        displayoffset|=DISPLAY_OFFSET;
        outputXML=OUTPUTLIBXML;
        ++number_of_processed_arguments;
    }
    
    if(arguments.count("-insxml") == 1) {
        displayoffset |=DISPLAY_OFFSET;
        outputXML      =OUTPUTLIBXML;
        ++number_of_processed_arguments;
    }
    
    if(arguments.count("-xml") == 1) {
        outputXML=OUI;
        ++number_of_processed_arguments;
    }


///////////////////////////////////////////////////////////////////////
// DEPENDENCES & DEPENDENCES DISPLAY

    if(arguments.count("-f") == 1) {
        dependencies=OUI;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-np") == 1) {
        dependcydisplay = AFFICHE_NUM_PHRASE;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-npi") == 1) {
        dependcydisplay = DEPENDENCY_SENTENCE_NUMBER_DISPLAY|DEPENDENCY_INVERTED_DISPLAY;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-feat") == 1) {
        dependcydisplay |= DEPENDENCY_SENTENCE_NUMBER_DISPLAY;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-disj") == 1) {
        dependcydisplay |= DEPENDENCY_INCOMPATIBLE_DISPLAY;
        ++number_of_processed_arguments;
    }



///////////////////////////////////////////////////////////////////////
// DISPLAY TREE
    if(arguments.count("-t") == 1) {
        show_tree = DISPLAY_TREE;
        ++number_of_processed_arguments;
    }
    
    if(arguments.count("-tc") == 1) {
        show_tree = DISPLAY_TREE_PHRASE;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-pythondebug") == 1) {
        num_fichier_python = atoi(strdup(argv[arguments["-pythondebug"] + 1]));
        num_python_ligne   = atoi(strdup(argv[arguments["-pythondebug"] + 2]));
        python_debug=OUI;
        number_of_processed_arguments += 3;
    }
    

//         if (!strcmp(argv[i],"-pythondebug")) {
//             if ((i+2)>=argc) {
//                 Usage();
//                 *flot_erreur<<endl<<"Please give first the module number then the line number"<<endl;                
//                 return -1;
//             }
//             else {                
//                 num_fichier_python=atoi(argv[++i]);
// 				num_python_ligne=atoi(argv[++i]);
// 				python_debug=OUI;
//             }
//             continue;
//         }

    if(arguments.count("-tl") == 1) {
        show_tree = DISPLAY_TREE_COLUMN;
        column    = atoi(strdup(argv[arguments["-tl"] + 1]));
        number_of_processed_arguments += 2;
    }

//         if (!strcmp(argv[i],"-tl")) {
//             if ((i+1)>=argc) {
//                 Usage();
//                 *flot_erreur<<endl<<"Please give a numeral"<<endl;                
//                 return -1;
//             }
//             else {
//                 de_tree=ARBRE_COLONNE;
//                 column=atoi(argv[++i]);
//             }
//             continue;
//         }


///////////////////////////////////////////////////////////////////////
// DISPLAY ERRORS & RESULTS
    if(arguments.count("-warning") == 1) {
        displayerror     = OUI;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-nodisplay") == 1) {
        displayresult     = NON;
        ++number_of_processed_arguments;
    }


///////////////////////////////////////////////////////////////////////
// INTERNAL STATISTICS

    if(arguments.count("-interrules") == 1) {
        statistics   = OUI;
        displayrules = 2;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-printrules") == 1) {
        statistics   = OUI;
        displayrules = OUI;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-time") == 1) {
        compute_time = OUI;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-detect") == 1) {
        compute_time = 2;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-stats") == 1) {
        statistics = OUI;
        compute_time = OUI;
        ++number_of_processed_arguments;
    }


///////////////////////////////////////////////////////////////////////
// DEPENDENCY ORDER

    if(arguments.count("-obn") == 1) {
        dependencyorder = DISPLAY_DEPENDENCY_BY_NODE;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-ord") == 1) {
        dependencyorder = DISPLAY_DEPENDENCY_BY_CREATION;
        ++number_of_processed_arguments;
    }


///////////////////////////////////////////////////////////////////////
// MATH

    if(arguments.count("-math") == 1) {
        math = OUI;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-mathonly") == 1) {
        math = 2;
        ++number_of_processed_arguments;
    }


///////////////////////////////////////////////////////////////////////
// AUTOMATON

    if(arguments.count("-automaton") == 1) {
        displayautom = OUI;
        ++number_of_processed_arguments;
    }


///////////////////////////////////////////////////////////////////////
// AUTOMATON

    if(arguments.count("-renum") == 1) {
        renumber = OUI;
        ++number_of_processed_arguments;
    }

    if(arguments.count("-nrenum") == 1) {
        renumber = NON;
        ++number_of_processed_arguments;
    }

///////////////////////////////////////////////////////////////////////
// INPUT

    if(arguments.count("-text") == 1) {
        std_input_ = false;
        filenames.push_back(string(argv[arguments["-text"] + 1]));
        number_of_processed_arguments += 2;
    }

    if(arguments.count("-texts") == 1) {
        std_input_ = false;
        ifstream fs(argv[arguments["-texts"] + 1]);
        while(not fs.eof()) {
            string s;
            getline(fs, s);
            if(not s.empty())
                filenames.push_back(s);
        }
        number_of_processed_arguments += 2;
    }

///////////////////////////////////////////////////////////////////////
    if(number_of_processed_arguments != argc - 1)
        success = false;
}


GlobalParseurTry::GlobalParseurTry(char* grammarfile,
                                   char* pathname)
    : GlobalParseur(grammarfile, pathname)
{}

int GlobalParseurTry::read(char* buffer,int size) {
    // if we have reached the end of the file, we return 0  
    // fichierDonnees is the handle on the current file, set with
    // FileAssociation

    if(feof(fichierDonnees))
    return 0;

    // we read a line from the file
    // each line correspond to a reading
    // Word readings are separated by line with only a '-'
    // Example:
    // The|the|0|3|Det|
    // -
    // tries|try|4|9|Noun|Pl|
    // tries|try|4|9|Verb|Sg|

    fgets(buffer,size,fichierDonnees);
    int end = strlen(buffer) - 1;
    while(end > 0 && buffer[end] < 32)
    buffer[end--] = 0;
    
    //if it is a "-", then we have found the reading separator
    if(!strcmp(buffer, "-") || strlen(buffer) < 2)
    return 2;
    return 1;
}

void GlobalParseurTry::LexiconEntryParsing(char* reading_string,
                       char** duplicat) {
    char* pt;
    
    //we reset the list of tags
    ListeTagsRAZ();
    
    //The number of reading
    nb_lectures=0;
    
    if (TestAffichage(AFFICHAGE_ENTREE) || DESAMBIGUISATION == OUI)
    *duplicat = DupliqueChaine(reading_string);
    else
    *duplicat = NULL;
    
    //We suppose the separator to be a "|" : surface|lemma|leftoffset|rightoffset|F1|F2|
    //The string also ends with a pipe in our example (to simplify the
    //job)

    char* start = reading_string;
    pt = strchr(start, '|');
    int counter = 0;
    while(pt != NULL) { 
    // we split at the "|"

    *pt = 0;
    
    // This If is used to break tokem separated with TOK
    if (DECOUPAGE!=NULL && !strcmp(DECOUPAGE,start)) {
         //nb_lectures counts the number of splits in your string
			//it means number of readings
			nb_lectures++;
		}

    ListeTagsAjouter(start);

    // The lemma form

    start = pt + 1;
    pt=strchr(start, '|');

    // By default, the morphological tags from the external lexicons are in lowercase, 
    //     execept if the NON_NORMALISATION_TAG is set.
    // We do not put in lowercase, the surface form and the lemma form, so we start
    //     testing after the second string.

    if(counter == 2 && !TestAffichage(NON_NORMALISATION_TAG))
        MetEnMinuscule((uchar*)start);
    ++counter;
    }
}

// Analyse d'une sequence de mots a partir d'un fichier...
void GlobalParseurTry::SentenceAnalysis(char renumerote, char arbre) {
    char buffer[1000];
    char* dupchaine;
    int bascule = 1;
    baseOffset = 0;
    char loop = 1;
    
    while(loop) {        
    // if the renumbering is requested, we set the word_number to 0

    if (renumerote==OUI)
        num_mot=0;
    else // otherwise we increment it
        num_mot++;
        
    ++nombre_mots;
    ListeVoc* voc = NULL;
        
    char testSentenceEnd = NON;
    char testMarkupTags  = NON;
        
    char internalloop = 1;
    loop = read(buffer, 1000);
    while(loop) {
        switch(loop) {
          case 0:
        break;
          case 1:
        testMarkupTags=TestBalise(buffer);
        if(testMarkupTags == NON) {
            //std::cerr<< buffer << std::endl;                 ////////////////////////////////////////////////////////////////////////////////
            testSentenceEnd = EndOfSentence(buffer, NON);
            LexiconEntryParsing(buffer, &dupchaine);
            if(voc == NULL) {
            voc = ConstruitForme(dupchaine, num_mot);
            if(voc != NULL) {
                num_mot++;
                nombre_mots++;
            }
            } else {
            FusionneForme(voc, dupchaine);
            }
        }
        break;
          case 2:
        voc = NULL;
        if(testMarkupTags == NON)
            gBaliseDebut->raz();
        }
            
        if (!loop || testSentenceEnd == OUI || EndOfSentence(buffer, UTILISE))
        break;
        loop = read(buffer, 1000);
    }
        
    // we then parse the sentence

    Parse(OUI);

    // we increment the sentence number

    ++num_phrase;

    // We then delete the intermediate representation
    Detruit();
    }
    
    // Analysis End reached
    FinAnalyse = OUI;
    while(phrase.dernier != 0) {

    // If there is still something to parse, we then parse again.

    Parse(OUI);
    Detruit();
    }
    FinAnalyse = NON;
}


void GlobalParseurTry::loading(char* path,
                               VECTA<char*>& fileparam,
                               Options const& options) {
                               

    typeLangue language = ANGLAIS; // Do not touch, always English (Anglais)

    char stringfile[] = ""; // useless, to replace the error messages
                            // and field names with new ones

    int numberValues = -1;  // better when specified in the grammar file
        
    char theTrace               = options.get_trace_level();
    char outputXML              = options.get_outputXML();
    char tagger                 = options.get_tagger();
    char tagging                = options.get_tagging();
    char math                   = options.get_math();           // To display numerical variables on screen
    char crypte                 = NON;                          // To encrypt a gramamr
    char display_error          = options.get_displayerror();   // To display warning messages
    char resultDisplay          = options.get_displayresult();; // To display result
    char dependency_extraction  = options.get_dependencies();

    TVecteur dependency_display = options.get_dependcyDisplay();
    TVecteur dependencyOrder    = options.get_dependcyOrder();
    TVecteur offsetDisplay      = options.get_displayoffset();
    TVecteur displaymode        = options.get_displaymode();
    TVecteur de_tree            = options.get_showtree();

    if (crypte==NON)
        TRACEREGLE = options.get_therules();

    long conversionutf8 = options.get_UTF8(); //0; // UTF8_INPUT, DISPLAY_CONVERSION_UTF8
    
    
    InitialisationParseur(language,
              stringfile,
              path,
              numberValues,
              theTrace,
              dependency_display,
              outputXML,
              tagger,
              tagging,
              math,
              crypte,
              display_error,
              resultDisplay,
              dependencyOrder,
              offsetDisplay,
              displaymode,
              de_tree,
              dependency_extraction,
              fileparam);
    
    ChoixUTF8(conversionutf8);
    GenereOptimise(NON);
}

inline void help(const char *progname)
{
    std::cerr
    << "Usage: " << progname << " -h" << std::endl
    << "       " << progname << " -d dir -s xip_spec [-i file]" << std::endl
    << std::endl
    << "L2F XIP. 2006/3/23" << std::endl
    << std::endl
    << "Processes 'file' or STDIN (if file not specified)."
    << " Output goes to STDOUT." << std::endl
    << " -d dir      Specifies the directory of xip specification." << std::endl
    << " -s xip_spec XIP specification, located in dir." << std::endl
    << std::endl
        <<"Usage:" <<endl
	 <<"     -help (prints out some information about XIP options)"<<endl
    <<"     -grm file (load a grammar file)"<<endl
    <<"     -english (comments and commands are in English"<<endl
    <<"     -french (comments and commands are in French"<<endl
    <<"     -string filename (personal string file)"<<endl
    <<"     -text filename (text to parse)"<<endl
    <<"     -texts filename (a file that comprises on each line a filename to parse)"<<endl
    <<"     -seek value (starts the analysis of a text file at value)"<<endl
    <<"     -ntm ntmscript"<<endl
    <<"     -ntmonly"<<endl
    <<"     -pathnode (displays the node with their complete path)"<<endl
    <<"     -hmm hmmfilename"<<endl
    <<"     -time (computes time of analysis)"<<endl
    <<"     -number value (overides field number in parameter file)"<<endl
    <<"     -g (parameter file for a given language)"<<endl
    <<"     -tagger (tagging mode, no parsing)"<<endl
    <<"     -tagging (disambiguation rules are used while parsing)"<<endl
    <<"     -ntagging (disambiguation rules are not used while parsing)"<<endl
    <<"     -a (for a full display of syntactic node information)"<<endl
    <<"     -r (for a reduced display of syntactic node information)"<<endl
    <<"     -tr (for an even more reduced display of syntactic node information)"<<endl
    <<"     -lem (same as -tr but with lemmas instead of surface form)"<<endl
    <<"     -cat (displays the leaf category. Sould be used AFTER -lem or -tr)"<<endl
    <<"     -disj (displays the disjunctive sets for incompatible dependencies)"<<endl
    <<"     -nm (displays the word number)"<<endl
    <<"     -obn (order by node, the dependencies are displayed according to their first parameter)"<<endl
    <<"     -ord (dependencies are ordered according to their creation)"<<endl
    <<"     -depfeatval (displays the value of the feature dependencies)"<<endl
    <<"     -t (display of the syntactic tree)"<<endl
    <<"     -tl number (automatically wraps the syntactic tree at 'number')"<<endl
    <<"     -tc (displays of the syntactic tree. Each sub-node under the top node is displayed independently)"<<endl
    <<"     -offset (display offset values for each token)"<<endl
    <<"     -sentence (display the sentence that has been analyzed)"<<endl
    <<"     -sentencenum (work with -sentence, display the sentence number with the sentence itself)"<<endl
    <<"     -ntree (the chunk tree is not displayed)"<<endl
    <<"     -prolog (dependencies are displayed in prolog form)"<<endl
    <<"     -inpututf8 (Uses this flag if the text is in UTF-8 format)"<<endl
    <<"     -outpututf8 (Uses this flag to output the text in UTF-8 format)"<<endl    
    <<"     -tokenize (Each line in the input file is a single token only valid with a NTM script)"<<endl    
    <<"     -languagechecking (The parser checks for each sentence whether it belongs to the current grammar language)"<<endl
    <<"     -no_normalization (Does not normalize the translation tags to lowercase)"<<endl
#if defined(WIN32)
    <<"     -dos (Translates characters in DOS format)"<<endl
#endif
    <<"     -f (extraction of the dependencies)"<<endl
    <<"     -nodisplay (only execution, no output)"<<endl
    <<"     -max nb (maximum number of sentences that will be analyzed"<<endl
    <<"     -trace (generates a trace in the file defined in the configuration file)"<<endl
    <<"     -dependency (generates a trace fils only for the dependency calculus)"<<endl
    <<"     -rules (generates a trace file containing rules and their actual index)"<<endl
    <<"     -automaton (generates a trace file with the rule automata inside)"<<endl
    <<"     -indent (generates a file containing the result of the parse as indented trees)"<<endl
    <<"     -ne  (displays a dependency with the list of rules that applied to create it)"<<endl
    <<"     -np (adds the phrase number in the dependency output, does not display the chunk tree)"<<endl
    <<"     -npi (same as -np but also displies an inverted form of the dependencies)"<<endl
    <<"     -feat (displays the feature structure for each token in tagger mode)"<<endl
    <<"     -addhmm (adds the hmm features for the non-selected readings after an HMM tagging)"<<endl
    <<"     -math (displays or transfers the result of mathematical formulae)"<<endl
    <<"     -mathonly (only display the result of mathematical formulae, it does not display other results)"<<endl
    <<"     -upto  label|num (run the grammar up to a certain label or a rule number)"<<endl
    <<"     -optimize file (generate a file with the optimized dependency rules)"<<endl
    <<"     -xml (the output is displayed as an xml output)"<<endl
#ifdef XIPLIBXML
    <<"     -xmldb alias filename"<<endl
    <<"\t\t   alias is the name through which the database is accessed in rules"<<endl
    <<"\t\t   filename the filename of the database"<<endl
    <<"     -xmltext depth filename (an xml file which will be parsed according to the XMLGUIDE provided by the grammar)"<<endl
    <<"     -xmltexts depth filename (a text file which contains xml filenames which will be parsed according to the XMLGUIDE provided by the grammar)"<<endl
    <<"\t\t   depth is used to split an XML document into chunks according to their depth,"<<endl
    <<"\t\t   it is especially useful to treat large XML document that could not be loaded at once in memory."<<endl
    <<"     -insxml inserts the result of the analysis into the xml file as output"<<endl
    <<"     -outxml inserts the result of the analysis into the xml file as output, the result is then displayed on screen"<<endl
    <<"     -txt2xml root encoding filename (creates a new xml file out of a text file with 'root' as the root markup tag and with a certain encoding)"<<endl
    <<"     -resultxmldir path (the path where result files are stored)"<<endl
#endif
    <<"     -markup (the content of the XML tags being analyzed is displayed on screen)"<<endl
    <<"     -warning (displays the unknown tag warnings (lexicon tags that have not been associated with a parser category)"<<endl
    <<"     -debug (displays the execution error of XIP instructions)."<<endl
    <<"     -sem filename (this command is used to create an index file)"<<endl
    <<"\t\t   The filename must contain semantic disambiguation rules."<<endl
    <<"\t\t   More than one file can be indexed in one step."<<endl
    <<"\t\t   The -index must be provided on the command line."<<endl
    <<"     -index filename (this command must be used together with -sem. The filename stores the index computed out of the -sem filenames)"<<endl
    <<"     -p filename (filename comprises new rules and definition to add to the kernel grammar)"<<endl
    <<"     -renum (for each new sentence, word number starts at 0)"<<endl
    <<"     -nrenum (the first word number of a new sentence continues those of the previous sentences)"<<endl    
    <<endl<<"     \"a sentence\" or < file"<<endl
    <<endl
    << "Please send error reports and comments to <rdmr@l2f.inesc-id.pt>."
    << std::endl;
}


int main(int argc, char* argv[])
{
    Options options(argc, argv);

    if(not options.good()) {
        std::cerr << "Invalid options." << std::endl << std::endl;
        help(argv[0]);
        return 0;
    }

    if(options.help()) {
        help(argv[0]);
        return 0;
    }

    // EXTERNAL VARIABLES
    affichage_supplementaire = options.get_displayother();
    affiche_regle_appliquee  = options.get_displayrules();
    afficheautomate          = options.get_display_autom();
    calcultemps              = options.get_compute_time();
    statistique_interne      = options.get_statistics();

    // to remove in the future
    char* xip_spec = options.xip_spec();
    char* dir      = options.directory();

    // Local variables
    time_t starttime, endtime;
    double duration;

    VECTA<char*> parameters;
    GlobalParseurTry* parser = new GlobalParseurTry(xip_spec, dir);
    parser->loading(dir, parameters, options);

    //This instruction set the value of fichierDonnees with text

    parser->TRACEDEPENDANCE    = options.get_traceDep();
    parser->TRACETRAITS        = options.get_tracefeat();
    parser->MaxSentence        = options.get_maxSentence();
    parser->colonne            = options.get_column();
    parser->appliqueGrammaire  = options.get_analysis();

//  parser->conversion         = options.get_convert();

    if(options.readFromFile()) {
        vector<string> files_to_read = options.filesToRead();
        cout << files_to_read.size() << endl;
        for(vector<string>::size_type i = 0;
            i != files_to_read.size();
            ++i) {
            FILE* text = fopen(files_to_read[i].c_str(), "r");
            if(text == NULL)
                std::cerr << "Could not open file <"
                          << files_to_read[i]
                          << ">." << std::endl;

			//This instruction sets the pathname of the current filename in GlobalParseur inner buffer
			//This pathname is then displayed on screen and is accessible through @pathname in the XIP script language
			parser->renseigne((char*)files_to_read[i].c_str());
			//if seek_value has been set, start the analysis of the file at this position within the file
			long seek_value=-1;
			if (seek_value!=-1)
				fseek(text,seek_value,SEEK_SET);
            parser->FileAssociation(text);
            parser->Analyse(options.get_renumber(), options.get_tree());
            fclose(text);
        }
    } else {
        parser->FileAssociation(stdin);
        parser->Analyse(options.get_renumber(), options.get_tree());
    }
    
    if(calcultemps != NON) {        
        float valreu;
        time(&endtime);
        duration = difftime(endtime, starttime);
        cout << "Duration : "  << duration << "s"     << endl;
        cout << "Words : "     << parser->nombre_mots << endl;
        cout << "Sentences : " << parser->num_phrase  << endl;
        if(duration > 0) {
            valreu = parser->nombre_mots / duration;
            cout << "Words/s : " << valreu << endl;
        }
        cout << "Number of dependencies extracted or modified: "
             << resolution_reussite << endl;
        cout << "Number of dependencies extracted: "
             << nb_dependances << endl
             << endl;
    }

	parser->AfficheLesStats();
	//XipFreeAllGrammars();    
	DetruitEcran();

    return 0;
}

// Comnments:
//      hmmselection is a pre-declared feature in XIP, which is used
//      to mark the reading that had been selected, but not deleted,
//      by the HMM.

