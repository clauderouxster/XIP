// l2f_xip.cpp : Defines the entry point for the console application.

// Compilation flags: XIPDLL_EXPORT, XIPSTLAPI, XIPSTLUSE
// Libraries: libxips.lib, libpython.a

#include "parsbin.h"
//#include "l2f_xip.h"
//
//extern char  affichage_supplementaire;
//extern char  affiche_regle_appliquee;
//extern char  afficheautomate;
//extern char  calcultemps;
//extern char  statistique_interne;
//extern float resolution_reussite;
//extern int   nb_dependances;
//
//void XipFreeAllGrammars();
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
//GlobalParseurTry::GlobalParseurTry(char* grammarfile, char* pathname) : GlobalParseur(grammarfile, pathname) {
////	 memset(dummyMemCheck, 0, 1000);
//}
//
//
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
//int GlobalParseurTry::read(char* buffer,int size) {
//	//if we have reached the end of the file, we return 0	
//	//fichierDonnees is the handle on the current file, set with FileAssociation
//	if (feof(fichierDonnees))
//		return 0;
//
//    // we read a line from the file
//    // each line correspond to a reading
//    // Word readings are separated by line with only a '-'
//    // Example:
//    // The|the|0|3|Det|
//    // -
//    // tries|try|4|9|Noun|Pl|
//    // tries|try|4|9|Verb|Sg|
//	buffer[0]=0;
//    fgets(buffer,size,fichierDonnees);
//    int end = strlen(buffer) - 1;
//    while(end > 0 && buffer[end] < 32)
//    buffer[end--] = 0;
//    
//    //if it is a "-", then we have found the reading separator
//	if (!strcmp(buffer,"-") || strlen(buffer)<2)
//		return 2;
//	return 1;
//}
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
////This function is used to split a string into surface,lemma and feature strings
////Which are then stored into ListeTags for further processing.
//void GlobalParseurTry::LexiconEntryParsing(char* reading_string,char** duplicat) {
//    char* pt;
//
//	//we reset the tag stack
//   ListeTagsRAZ(reading_string,duplicat);
//	int counter=0;
//	int first=0;
//
//	//We suppose the separator to be a "|" : surface|lemma|leftoffset|rightoffset|F1|F2|
//	//The string also ends with a pipe in our example (to simplify the job)
//	char* start=reading_string;
//
//	//IMPORTANT: A specific case: The first character is a pipe... In that case, we expect the LEMMA and the SURFACE
//	//to have THE SAME SIZE...
//	if (start[0]=='|') {
//		//we read all the pipe until we find a non pipe
//		int p=0;
//		while (*start++ =='|') p++;
//		start--;
//		//we split the string in two minus 2 (the two separator pipes)
//		p-=2;
//		//we then divide in two equal parts
//		p=p/2;
//		//we then extract our string...
//		reading_string[p]=0;
//		//We add twice the same string, we suppose surface and lemma to be similar...
//		ListeTagsAjouter(reading_string);
//		ListeTagsAjouter(reading_string);
//		first++;
//		counter=2;
//	}
//
//	pt=strchr(start,'|');
//	while (pt!=NULL) {	
//		//we split at the "|"
//		*pt=0;
//		//We need a specific processing to handle the Split Category
//		CategorySplitProcessing(start);
//		//We add our tag to the feature stack
//		ListeTagsAjouter(start);
//		
//		//ADDED on 2011/06/16
//		//So the correct structure to send to XIP now is:    |raw surface|surface|lemma|F1|F2...
//		if (first == 0) {
//		   first++;
//         ListeTagsAjouter(start);  
//		}
//		//we jump to the next "|"
//		start=pt+1;
//		pt=strchr(start,'|');
//		//By default, the morphological tags from the external lexicons are in lowercase, except if the NON_NORMALISATION_TAG is set.
//		//We do not put in lowercase, the surface form and the lemma form, so we start after the second string.
//		//SetLowerCase tests the NON_NORMALISATION_TAG in order to set the string in lower case or not
//      if (enregListeTags.dernier==3 && !TestOptions(NON_NORMALISATION_TAG)) {
//         MetEnMinuscule((uchar*)start);
//	   }
//   }
//	//We have a last test, which consists of spreading HMM value accross the different reading extracted from
//	//the string
//	HMMInsertionProcessing();
//}
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
////Main loop to analyze a sentence extracted from a file or a string
//void GlobalParseurTry::SentenceAnalysis(char renumber,char tree) {
//    char buffer[1000];
//    char* dupchaine;
//	char loop=YES;
//
//    while (loop) {        
//		//if the renumbering is requested, we set the word_number to 0
//		SetWordNumber(renumber);        
//        ListeVoc* voc=NULL;
//
//        char testSentenceEnd=NO;
//        char testMarkupTags=NO;
//
//		loop=read(buffer,1000);
//        while (loop) {
//			switch(loop) {
//				case 0:
//					break;
//				case 1:
//					testMarkupTags=TestMarkupTag(buffer);
//					if (testMarkupTags==NO) {
//						testSentenceEnd=EndOfSentence(buffer,NO);
//						LexiconEntryParsing(buffer,&dupchaine);
//						if (voc==NULL) {
//							//This is the initialisation of your reading set
//							//The first reading should be built with BuildFirstReading
//							voc=BuildFirstReading(dupchaine,num_mot);
//							if (voc!=NULL)
//								IncrementWordIndexes();
//						}
//						else
//							//If it is not the first reading, then you simple
//							//merge it into your voc variable...
//							AddToCurrentReading(voc,dupchaine);                    
//					}         
//					break;
//				case 2:
//					voc=NULL;
//					if (testMarkupTags==NO)
//						ResetMarkuptags();						
//			}
//
//            if (!loop || testSentenceEnd==YES || EndOfSentence(buffer,UTILISE))
//                break;
//			loop=read(buffer,1000);
//        }
//        
//		//we then parse the sentence
//        Parse(YES);
//		//we increment the sentence number
//        IncrementSentence();
//        //We then delete the intermediate representation
//        Clean();
//    }
//
//	//Analysis End reached
//    SetEndOfAnalysis(YES);
//    while (phrase.dernier!=0) {
//		//If there is still something to parse, we then parse again..
//        Parse(YES);
//        Clean();
//    }
//    SetEndOfAnalysis(NO);
//}
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
//int GlobalParseurTry::loading(char* path,VECTA<char*>& fileparam, Options const& options) {
//
//	typeLangue language=ANGLAIS; // Do not touch, always English (Anglais)
//   char stringfile[] = "";     // useless, to replace the error messages and field names with new ones
//   int numberValues = -1;      // better when specified in the grammar file
//    
//    char theTrace               = options.get_trace_level();
//    char outputXML              = options.get_outputXML();
//    char tagger                 = options.get_tagger();
//    char tagging                = options.get_tagging();
//    char math                   = options.get_math();           // To display numerical variables on screen
//    char crypte                 = options.get_crypte();         // To use an encrypted grammar
//    char display_error          = options.get_displayerror();   // To display warning messages
//    char resultDisplay          = options.get_displayresult();; // To display result
//    char dependency_extraction  = options.get_dependencies();
//
//    TVecteur dependency_display = options.get_dependcyDisplay();
//    TVecteur dependencyOrder    = options.get_dependcyOrder();
//    TVecteur offsetDisplay      = options.get_displayoffset();
//    TVecteur displaymode        = options.get_displaymode();
//    TVecteur de_tree            = options.get_showtree();
//
//	TRACEDEPENDANCE=NO;
//	TRACETRAITS=NO;
//
//    if (crypte==NON)
//        TRACEREGLE = options.get_therules();
//
//    long conversionutf8 = options.get_UTF8(); //0; // UTF8_INPUT, DISPLAY_CONVERSION_UTF8
//   
//    int ipar=InitialisationParseur(language,
//              stringfile,
//              path,
//              numberValues,
//              theTrace,
//              dependency_display,
//              outputXML,
//              tagger,
//              tagging,
//              math,
//              crypte,
//              display_error,
//              resultDisplay,
//              dependencyOrder,
//              offsetDisplay,
//              displaymode,
//              de_tree,
//              dependency_extraction,
//              fileparam);
//    
//    ChoixUTF8(conversionutf8);
//    GenereOptimise(NON);
//    return ipar;
//}
//
//
//
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
//inline void help(const char *progname) {
//    std::cerr
//    << "Usage: " << progname << " -h" << std::endl
//    << "       " << progname << " -d dir -s xip_spec [-i file]" << std::endl
//    << std::endl
//    << "L2F XIP. 2006/3/23" << std::endl
//    << std::endl
//    << "Processes 'file' or STDIN (if file not specified)."
//    << " Output goes to STDOUT." << std::endl
//    << " -d dir      Specifies the directory of xip specification." << std::endl
//    << " -s xip_spec XIP specification, located in dir." << std::endl
//    << std::endl
//    <<"Usage:" <<endl
//	 <<"     -help (prints out some information about XIP options)"<<endl
//    <<"     -grm file (load a grammar file)"<<endl
//    <<"     -string filename (personal string file)"<<endl
//    <<"     -text filename (text to parse)"<<endl
//    <<"     -texts filename (a file that comprises on each line a filename to parse)"<<endl
//    <<"     -seek value (starts the analysis of a text file at value)"<<endl
//    <<"     -ntm ntmscript"<<endl
//    <<"     -ntmonly"<<endl
//    <<"     -pathnode (displays the node with their complete path)"<<endl
//    <<"     -hmm hmmfilename"<<endl
//    <<"     -time (computes time of analysis)"<<endl
//    <<"     -tagger (tagging mode, no parsing)"<<endl
//    <<"     -tagging (disambiguation rules are used while parsing)"<<endl
//    <<"     -ntagging (disambiguation rules are not used while parsing)"<<endl
//    <<"     -a (for a full display of syntactic node information)"<<endl
//    <<"     -r (for a reduced display of syntactic node information)"<<endl
//    <<"     -tr (for an even more reduced display of syntactic node information)"<<endl
//    <<"     -lem (same as -tr but with lemmas instead of surface form)"<<endl
//    <<"     -cat (displays the leaf category. Sould be used AFTER -lem or -tr)"<<endl
//    <<"     -disj (displays the disjunctive sets for incompatible dependencies)"<<endl
//    <<"     -nm (displays the word number)"<<endl
//    <<"     -obn (order by node, the dependencies are displayed according to their first parameter)"<<endl
//    <<"     -ord (dependencies are ordered according to their creation)"<<endl
//    <<"     -depfeatval (displays the value of the feature dependencies)"<<endl
//    <<"     -t (display of the syntactic tree)"<<endl
//    <<"     -ntree (the chunk tree is not displayed)"<<endl
//    <<"     -tl number (automatically wraps the syntactic tree at 'number')"<<endl
//    <<"     -tc (displays of the syntactic tree. Each sub-node under the top node is displayed independently)"<<endl
//    <<"     -offset (display offset values for each token)"<<endl
//    <<"     -sentence (display the sentence that has been analyzed)"<<endl
//    <<"     -sentencenum (work with -sentence, display the sentence number with the sentence itself)"<<endl
//    <<"     -prolog (dependencies are displayed in prolog form)"<<endl
//    <<"     -inpututf8 (Uses this flag if the text is in UTF-8 format)"<<endl
//    <<"     -outpututf8 (Uses this flag to output the text in UTF-8 format)"<<endl    
//    <<"     -tokenize (Each line in the input file is a single token only valid with a NTM script)"<<endl    
//    <<"     -languagechecking (The parser checks for each sentence whether it belongs to the current grammar language)"<<endl
//    <<"     -no_normalization (Does not normalize the translation tags to lowercase)"<<endl
//#if defined(WIN32)
//    <<"     -dos (Translates characters in DOS format)"<<endl
//#endif
//    <<"     -f (extraction of the dependencies)"<<endl
//    <<"     -nodisplay (only execution, no output)"<<endl
//    <<"     -printrules (displays the rules that have actually applied on a given sentence (very useful for debugging))"<<endl
//    <<"     -interrules (displays the rules that have not applied on a given sentence)"<<endl
//    <<"     -max nb (maximum number of sentences that will be analyzed"<<endl
//    <<"     -trace (generates a trace in the file defined in the configuration file)"<<endl
//    <<"     -dependency (generates a trace fils only for the dependency calculus)"<<endl
//    <<"     -rules (generates a trace file containing rules and their actual index)"<<endl
//    <<"     -automaton (generates a trace file with the rule automata inside)"<<endl
//    <<"     -indent (generates a file containing the result of the parse as indented trees)"<<endl
//    <<"     -ne  (displays a dependency with the list of rules that applied to create it)"<<endl
//    <<"     -np (adds the phrase number in the dependency output, does not display the chunk tree)"<<endl
//    <<"     -npi (same as -np but also displies an inverted form of the dependencies)"<<endl
//    <<"     -feat (displays the feature structure for each token in tagger mode)"<<endl
//    <<"     -addhmm (adds the hmm features for the non-selected readings after an HMM tagging)"<<endl
//    <<"     -math (displays or transfers the result of mathematical formulae)"<<endl
//    <<"     -mathonly (only display the result of mathematical formulae, it does not display other results)"<<endl
//    <<"     -upto  label|num (run the grammar up to a certain label or a rule number)"<<endl
//    <<"     -optimize file (generate a file with the optimized dependency rules)"<<endl
//    <<"     -xml (the output is displayed as an xml output)"<<endl
//#ifdef XIPLIBXML
//    <<"     -xmldb alias filename"<<endl
//    <<"\t\t   alias is the name through which the database is accessed in rules"<<endl
//    <<"\t\t   filename the filename of the database"<<endl
//    <<"     -xmltext depth filename (an xml file which will be parsed according to the XMLGUIDE provided by the grammar)"<<endl
//    <<"     -xmltexts depth filename (a text file which contains xml filenames which will be parsed according to the XMLGUIDE provided by the grammar)"<<endl
//    <<"\t\t   depth is used to split an XML document into chunks according to their depth,"<<endl
//    <<"\t\t   it is especially useful to treat large XML document that could not be loaded at once in memory."<<endl
//    <<"     -insxml inserts the result of the analysis into the xml file as output"<<endl
//    <<"     -outxml inserts the result of the analysis into the xml file as output, the result is then displayed on screen"<<endl
//    <<"     -txt2xml root encoding filename (creates a new xml file out of a text file with 'root' as the root markup tag and with a certain encoding)"<<endl
//    <<"     -resultxmldir path (the path where result files are stored)"<<endl
//#endif
//    <<"     -markup (the content of the XML tags being analyzed is displayed on screen)"<<endl
//    <<"     -warning (displays the unknown tag warnings (lexicon tags that have not been associated with a parser category)"<<endl
//    <<"     -debug (displays the execution error of XIP instructions)."<<endl
//    <<"     -sem filename (this command is used to create an index file)"<<endl
//    <<"\t\t   The filename must contain semantic disambiguation rules."<<endl
//    <<"\t\t   More than one file can be indexed in one step."<<endl
//    <<"\t\t   The -index must be provided on the command line."<<endl
//    <<"     -index filename (this command must be used together with -sem. The filename stores the index computed out of the -sem filenames)"<<endl
//    <<"     -p filename (filename comprises new rules and definition to add to the kernel grammar)"<<endl
//    <<"     -renum (for each new sentence, word number starts at 0)"<<endl
//    <<"     -nrenum (the first word number of a new sentence continues those of the previous sentences)"<<endl    
//    <<endl<<"     \"a sentence\" or < file"<<endl
//    <<endl
//    << "Please send error reports and comments to <rdmr@l2f.inesc-id.pt>."
//    << std::endl;
//}
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
////The LoadGrm function can be enriched with many other options and many more parameters...
////In this current implementation, it only reads a grammar pathname and some environment variables...
////The method: ResetEnvironmentVariables creates also the default: GRM_PATHNAME variable, which contains the
////the GRM pathname.
//char LoadGrm(string grmname,string& grammar) {
//	char bufread[1000];
// 	char crypte=NO;
//	ifstream grm((char*)grmname.c_str(),modeOuverture);
//    if (grm.fail()) {
////        sprintf(bufread,"Cannot open: %s",grmname);
//        send_error(strdup(bufread));
//    }
//	
//	//This function is exported by the XIP library
//	ResetEnvironmentVariables(grmname);
//	
//
////----------------------------------------------------------------
//    while (!grm.eof()) {
//		//We read a line a trim it... This function also replaces the Environment variables
//		//with their values...
//        GrmTrim(grm,(uchar*)bufread,999,NO);
//        
//        if (!*bufread)
//            break;
//
//		//If it is a path (the definition of a path variable)
//		if (!strcmp(bufread,"path:")) {
//			ReadPathValue(grm);
//			continue;
//		}
//
//		//We then read the grammar pathname
//		if (!strcmp(bufread,"grammar:")) {
//			if (grammar!="") {
//				send_error("A GRAMMAR file has already been provided");
//			}
//            
//			GrmTrim(grm,(uchar*)bufread,999,YES);
//            if (bufread[0]!=0) {
//                if (strlen(bufread)>999) {
//                    send_error("Pathname for GRAMMAR too long");
//                }
//                grammar=bufread;
//            }
//            continue;
//        }
//		//We then read the grammar pathname, gram in that case is equivalent to grammar
//		//with an important modification: the grammar is encrypted.
//		if (!strcmp(bufread,"gram:")) {
//			if (grammar[0]) {
//				send_error("A GRAMMAR file has already been provided");
//			}
//            crypte=YES;
//			GrmTrim(grm,(uchar*)bufread,999,YES);
//            if (bufread[0]!=0) {
//                if (strlen(bufread)>999) {
//                    send_error("Pathname for GRAMMAR too long");
//                }
//                grammar=bufread;
//            }
//            continue;
//        }
//
//		//You can add as many options as you like...
//	}
//	return crypte;
//}
//
//
//
//
//
//
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
//Exportation int XipLocalGrmFile(string grmname,char loadall,int xml,Options const& options) {
//
//	//The loading should take place here...
//	string grammar;
//	string path;
//	string name;
//	
//// 	    if(xip_spec == 0 or dir == 0) {
////         std::cerr << "Directory of XIP specification or XIP specification not defined." << std::endl
////                   << std::endl
////                   << "Usage: " << argv[0] << " -h" << std::endl
////                   << "       " << argv[0] << " -d dir -s xip_spec [-i file]" << std::endl
////                   << std::endl;
////         return 0;
////     }
//	
//
//	//First we load the grammar name
//	//LoadGrm could have as many parameters as you like
//	char crypte=LoadGrm(grmname,grammar);
//	//XIP exports a function to separate the path from the name... It is of course platform dependent
//	CutPathFromName(grammar,path,name);
//	
//	GlobalParseurTry* parser=new GlobalParseurTry((char*)name.c_str(),(char*)path.c_str());
//	
//	//Then we load the grammar itself...
//	VECTA<char*> parameters;
//	int ipar=parser->loading((char*)path.c_str(),parameters,options);
//   return ipar;
//}
//
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
//
//
//
//
//
//
//
//
////################################################################################
////################################################################################
////################################################################################
////################################################################################
//int main(int argc, char* argv[]) {
//
//    Options options(argc, argv);
//
//    if(not options.good()) {
//        std::cerr << "Invalid options." << std::endl << std::endl;
//        help(argv[0]);
//        return 0;
//    }
//
//    if(options.help()) {
//        help(argv[0]);
//        return 0;
//    }
//
//    // EXTERNAL VARIABLES
//	affiche_regle_appliquee  = 0;
//    affichage_supplementaire = options.get_displayother();
//    afficheautomate          = options.get_display_autom();
//    calcultemps              = options.get_compute_time();
//    statistique_interne      = options.get_statistics();
//	calcultemps=1;
//    // to remove in the future
//    char* xip_spec = options.xip_spec();
//    char* dir      = options.directory();
//
//    // Local variables
//    time_t starttime, endtime;
//    double duration;
//	time(&starttime);
//
// //    VECTA<char*> parameters;
//  //   GlobalParseurTry* parser = new GlobalParseurTry(xip_spec, dir);
//  //  int ipar=parser->loading(dir, parameters, options);
//    
//   //int ipar=XipGrmFile("/afs/l2f.inesc-id.pt/projects/tools/xip/ptGram/GRMFILES/portuguese.grm ",1,0); //options.get_outputXML());
//	int ipar=XipLocalGrmFile("W:\\users\\Roux\\xip\\parSem\\xip\\dev\\GRAMMARS\\PORTUGUESE\\GRMFILES\\portuguese.grm",1,0,options); 
//    GlobalParseur* parser=XipSelectGrammar(ipar);
//
//    //This instruction set the value of fichierDonnees with text
//
//    parser->TRACEDEPENDANCE    = options.get_traceDep();
//    parser->TRACETRAITS        = options.get_tracefeat();
//    parser->MaxSentence        = options.get_maxSentence();
//    parser->colonne            = options.get_column();
//    parser->appliqueGrammaire  = options.get_analysis();
//
//
//
//
////  parser->conversion         = options.get_convert();
//
//    if(options.readFromFile()) {
//        vector<string> files_to_read = options.filesToRead();
//        cout << files_to_read.size() << endl;
//        for(vector<string>::size_type i = 0;
//            i != files_to_read.size();
//            ++i) {
//            FILE* text = fopen(files_to_read[i].c_str(), "r");
//            if(text == NULL)
//                std::cerr << "Could not open file <"
//                          << files_to_read[i]
//                          << ">." << std::endl;
//            parser->FileAssociation(text);
//
//			//This instruction sets the pathname of the current filename in GlobalParseur inner buffer
//		
//         //This pathname is then displayed on screen and is accessible through @pathname in the XIP script language
//
//         //  parser->renseigne((char*)files_to_read[i].c_str());
//         //if seek_value has been set, start the analysis of the file at this position within the file
//			
//         //    long seek_value=-1;
//	      // if (seek_value!=-1) fseek(text,seek_value,SEEK_SET);
//            parser->Analyse(options.get_renumber(), options.get_tree());
//            fclose(text);
//        }
//    } else {
//        parser->FileAssociation(stdin);
//        parser->Analyse(options.get_renumber(), options.get_tree());
//    }
//    
//    if(calcultemps != NON) {        
//        float valreu;
//        time(&endtime);
//        duration = difftime(endtime, starttime);
//        cout << "Duration : "  << duration << "s"     << endl;
//        cout << "Words : "     << parser->nombre_mots << endl;
//        cout << "Sentences : " << parser->num_phrase  << endl;
//        if(duration > 0) {
//            valreu = parser->nombre_mots / duration;
//            cout << "Words/s : " << valreu << endl;
//        }
//        cout << "Number of dependencies extracted or modified: "
//             << resolution_reussite << endl;
//        cout << "Number of dependencies extracted: "
//             << nb_dependances << endl
//             << endl;
//    }
//
//	affiche_regle_appliquee  = options.get_displayrules();
//	//XipFreeGrammar(ipar);
//    return 0;
//}
//


void monappel(int handle,XipResult* r,void* data) {
	cout<<r->sentences[0]->dependencies[0]->name<<endl;
	XipCleanCurrentXipResult(handle);
}


int main(int argc, char* argv[]) {
	int handle;

	try {
		handle=XipGrmFile("W:\\users\\grondeau\\parSem\\xip\\dev\\GRAMMARS\\ENGLISH\\GRMFILES\\GRAM_TEMP\\gram_temp.grm");
	}
	catch(XIPError* err) {
		cerr<<err->message<<" dans "<<err->filename<<endl;
	}

	XipSetCallBack(handle,monappel,NULL);
	XipResult* r=XipCreateCurrentXipResult(handle);
	XipAddFlagDisplayMode(handle,CHECK_INPUT_UTF8);
	XipParseString("The lady is using clichés in her speech.",handle,r,1);
	cout<<handle<<endl;
	XipDeleteCurrentXipResult(handle);
	XipFreeGrammar(handle);
}
