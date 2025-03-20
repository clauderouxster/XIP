#define not !
class Options {
public:
    Options(int const argc, char **argv);

    // Queries for XIP options ------------------------------------------------|

    int      get_column()        const { return column;          }
    int      get_maxSentence()   const { return maxSentence;     }

    long     get_UTF8()          const { return utf8;            }

    char     get_analysis()      const { return analysis;        }
    char     get_compute_time()  const { return compute_time;    }
    char     get_convert()       const { return convert;         }
    char     get_dependencies()  const { return dependencies;    }
    char     get_display_autom() const { return displayautom;    }
    char     get_displayerror()  const { return displayerror;    }
    char     get_displayother()  const { return displayother;    }
    char     get_displayresult() const { return displayresult;   }
    char     get_displayrules()  const { return displayrules;    }
    char     get_math()          const { return math;            }
    char     get_outputXML()     const { return outputXML;       }
    char     get_renumber()      const { return renumber;        }
    char     get_statistics()    const { return statistics;      }
    char     get_tagger()        const { return tagger;          }
    char     get_tagging()       const { return tagging;         }
    char     get_therules()      const { return therules;        }
    char     get_trace_level()   const { return trace_level;     }
    char     get_traceDep()      const { return tracedependency; }
    char     get_tracefeat()     const { return tracefeatures;   }
    char     get_tree()          const { return tree;            }
    char     get_crypte()        const { return crypte;          }

    TVecteur get_dependcyDisplay() const { return dependcydisplay; }
    TVecteur get_dependcyOrder()   const { return dependencyorder; }
    TVecteur get_displaymode()     const { return displaymode;     }
    TVecteur get_displayoffset()   const { return displayoffset;   }
    TVecteur get_showtree()        const { return show_tree;       }

    // ------------------------------------------------------------------------|

    char* directory() const { return dir; }
    char* xip_spec() const { return xip_spec_; }

    bool good() const         { return success;        }
    bool help() const         { return help_;          }
    bool readFromFile() const { return not std_input_; }

    vector<string> filesToRead() const { return filenames; }
    
private:
    static TVecteur const displaymode_complete;

    char analysis;        // ::: OUI, NON
    char compute_time;    // ::: OUI, NON
    char convert;         // ::: OUI, NON
    char dependencies;    // ::: OUI, NON
    char displayautom;    // ::: OUI, NON
    char displayerror;    // ::: OUI, NON
    char displayother;    // ::: 0, 1
    char displayresult;   // ::: OUI, NON
    char displayrules;    // ::: OUI, NON, 2
    char math;            // ::: OUI, NON, 2
    char outputXML;       // ::: OUI, NON
    char renumber;        // ::: OUI, NON
    char statistics;      // ::: OUI, NON
    char tagger;          // ::: OUI, NON
    char tagging;         // ::: OUI, NON
    char therules;        // ::: NON, 1
    char trace_level;     // ::: 0, 1, 10, 11
    char tracedependency; // ::: OUI, NON
    char tracefeatures;   // ::: OUI, NON
    char tree;            // ::: OUI, NON
    char crypte;	        // ::: OUI, NON
    
    TVecteur displaymode;     // ::: DISPLAY_LEMMA, DISPLAY_SURFACE, DISPLAY_MARKUP,
                              //     DISPLAY_ENTREE, DISPLAY_CATEGORY, DISPLAY_CHUNK_TREE
                              //     DISPLAY_REDUCED, DISPLAY_FULL, DISPLAY_OFFSET,
                              //     DISPLAY_WORD_NUM, DISPLAY_SENTENCE, DISPLAY_EXECUTION_ERROR
    TVecteur displayoffset;   // ::: DISPLAY_OFFSET
    TVecteur show_tree;       // ::: 0, DISPLAY_TREE, DISPLAY_TREE_PHRASE, DISPLAY_TREE_COLUMN
    TVecteur dependencyorder; // ::: DISPLAY_DEPENDENCY_BY_NAME, DISPLAY_DEPENDENCY_BY_NODE,
                              //     DISPLAY_DEPENDENCY_BY_CREATION;
    TVecteur dependcydisplay; // ::: DEPENDENCY_FUNCTION_DISPLAY, 
                              //     DEPENDENCY_SENTENCE_NUMBER_DISPLAY, 
                              //     DEPENDENCY_INVERTED_DISPLAY,
                              //     DEPENDENCY_FEATURE_DISPLAY,
                              //     DEPENDENCY_INCOMPATIBLE_DISPLAY 32

    int column;
    int maxSentence;
    
    long utf8;                // ::: 0, DISPLAY_CONVERSION_UTF8

    char* xip_spec_;
    char* dir;
    
    bool success;
    bool help_;
    bool std_input_;

    vector<string> filenames;
};

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
      crypte(NON), 
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

    if (arguments.count("-crypte") == 1) {
        crypte = OUI;
	++number_of_processed_arguments;
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

//    if(arguments.count("-pythondebug") == 1) {
//        num_fichier_python = atoi(strdup(argv[arguments["-pythondebug"] + 1]));
//	      num_python_ligne   = atoi(strdup(argv[arguments["-pythondebug"] + 2]));
//        python_debug=OUI;
//        number_of_processed_arguments += 3;
//    }
    
    if(arguments.count("-tl") == 1) {
        show_tree = DISPLAY_TREE_COLUMN;
        column    = atoi(strdup(argv[arguments["-tl"] + 1]));
        number_of_processed_arguments += 2;
    }



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
