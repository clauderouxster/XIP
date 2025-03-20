// derivation.cpp : Defines the entry point for the console application.
//

//Compilation flags: XIPDLL_EXPORT, XIPSTLAPI, XIPSTLUSE, XIPDERIVATIONMAIN
//Libraries: libxips.lib, libpython.a

#include "parsbin.h"
#include "derivation.h"


/*=====================================================================
=====================================================================
The GlobalParseur derivation implies the reimplementation of the following methods:

WARNING: This file contains an example of a derivation of GlobalParseur. It should not be considered as a 
	definitive way to handle reading strings. For instance, the use of "|" to separate characters might 
	prove too feable a solution for a real case application. Choose your own reading description according 
	to your needs.  

- The constructor GlobalParseurTry with the following two variables:
	grammarfile is the XIP language file which manages all other grammar file
	pathname is the path of that file

- The destructor ~GlobalParseurTry: if you have implemented some specific variables, 
	this is the place to clean them.

- LexiconEntryParsing(char* reading_string,char** duplicat): this method takes as input one reading as a string and split it into
	substrings. This reading should contain a surface form, a list of offset positions, a lemma form and a list of features.
	All these strings are stored individualy in a list with the method: ListeTagsAjouter.
	Some specific treatments are necessary to handle the Category Split feature, 
	with the method: CategorySplitProcessing, which should be called for each new substring.
	A call to HMMInsertionProcessing should end the whole process.
	In our example, we expect reading strings to use "|" as a separator. This choice can prove dangerous in 
	certain	cases, if your reading string already contains this "|".

- SentenceAnalysis: this method is called from the library to launch the reading of a text or of a file which
	will be tokenized and morpho-analyzed. In this example, it calls "read" which corresponds to your own 
	implementation of a tokenizer and of a morphological analyzer. In our example, we suppose that the system
	returns reading strings, where each element is separated with a "|".

- loading: This method can have any name you like, as long as it call the InitialisationParseur method to load 
	a grammar in memory.

The next two functions MUST BE reimplemented according to the model below
  These functions are:

	- XipGrmFile which is used to reload a grammar from a GRM file. This function is also used
	in XIPUI to load a grammar. Its reimplementation enables the new libreary to be implemented without
	too many problems into the JAR

	- Whoami: This function defines the output string that is used to discriminate between different XIP
	derivations.

*/

GlobalParseurTry::GlobalParseurTry(char* grammarfile,char* pathname):GlobalParseur(grammarfile,pathname) {
	 memset(dummyMemCheck, 0, 1000);
}

GlobalParseurTry::~GlobalParseurTry() {
	cout<<"ICI";
	cout<<endl;
}

int GlobalParseurTry::read(char* buffer,int size) {
	//if we have reached the end of the file, we return 0	
	//fichierDonnees is the handle on the current file, set with FileAssociation
	if (feof(fichierDonnees))
		return 0;
	//we read a line from the file
	//each line correspond to a reading
	//Word readings are separated by line with only a '-'
	//Example:
	//The|the|0|3|Det|
	//-
	//tries|try|4|9|Noun|Pl|
	//tries|try|4|9|Verb|Sg|
	fgets(buffer,size,fichierDonnees);
	int end=strlen(buffer)-1;
	while (end>0 && buffer[end]<32)
		buffer[end--]=0;
		
	//if it is a "-", then we have found the reading separator
	if (!strcmp(buffer,"-") || strlen(buffer)<2)
		return 2;
	return 1;
}
//-------------------------------------------------------------------------------
//This function is used to split a string into surface,lemma and feature strings
//Which are then stored into ListeTags for further processing.
void GlobalParseurTry::LexiconEntryParsing(char* reading_string,char** duplicat) {
    char* pt;

	//we reset the tag stack
    ListeTagsRAZ(reading_string,duplicat);
	
	//We suppose the separator to be a "|" : surface|lemma|leftoffset|rightoffset|F1|F2|
	//The string also ends with a pipe in our example (to simplify the job)
	char* start=reading_string;
	pt=strchr(start,'|');
	while (pt!=NULL) {	
		//we split at the "|"
		*pt=0;
		//We need a specific processing to handle the Split Category
		CategorySplitProcessing(start);
		//We add our tag to the feature stack
		ListeTagsAjouter(start);		
		//we jump to the next "|"
		start=pt+1;
		pt=strchr(start,'|');
		//By default, the morphological tags from the external lexicons are in lowercase, except 
		//if the NON_NORMALISATION_TAG is set.
		//We do not put in lowercase, the surface form and the lemma form, 
		//so we start after the second string.
		//SetLowerCase tests the NON_NORMALISATION_TAG in order to set the string in lower case or not
		SetLowerCase((uchar*)start);
	}
	//We have a last test, which consists of spreading HMM value accross the different reading extracted from
	//the string
	HMMInsertionProcessing();
}

//Main loop to analyze a sentence extracted from a file or a string
void GlobalParseurTry::SentenceAnalysis(char renumber,char tree) {
    char buffer[1000];
    char* dupchaine;
	char loop=YES;

    while (loop) {        
		//if the renumbering is requested, we set the word_number to 0
		SetWordNumber(renumber);        
        ListeVoc* voc=NULL;

        char testSentenceEnd=NO;
        char testMarkupTags=NO;

		loop=read(buffer,1000);
        while (loop) {
			switch(loop) {
				case 0:
					break;
				case 1:
					testMarkupTags=TestMarkupTag(buffer);
					if (testMarkupTags==NO) {
						testSentenceEnd=EndOfSentence(buffer,NO);
						LexiconEntryParsing(buffer,&dupchaine);
						if (voc==NULL) {
							//This is the initialisation of your reading set
							//The first reading should be built with BuildFirstReading
							voc=BuildFirstReading(dupchaine,num_mot);
							if (voc!=NULL)
								IncrementWordIndexes();
						}
						else
							//If it is not the first reading, then you simple
							//merge it into your voc variable...
							AddToCurrentReading(voc,dupchaine);                    
					}         
					break;
				case 2:
					voc=NULL;
					if (testMarkupTags==NO)
						ResetMarkuptags();						
			}

            if (!loop || testSentenceEnd==YES || EndOfSentence(buffer,UTILISE))
                break;
			loop=read(buffer,1000);
        }
        
		//we then parse the sentence
        Parse(YES);
		//we increment the sentence number
        IncrementSentence();
        //We then delete the intermediate representation
        Clean();
    }

	//Analysis End reached
    SetEndOfAnalysis(YES);
    while (phrase.dernier!=0) {
		//If there is still something to parse, we then parse again..
        Parse(YES);
        Clean();
    }
    SetEndOfAnalysis(NO);
}


int GlobalParseurTry::loading(char* path,char crypte,VECTA<char*>& fileparam) {

	typeLangue language=ANGLAIS; //Do not touch, always English (Anglais)
	char stringfile[]=""; //useless, to replace the error messages and field names with new ones
	int numberValues=-1; // better when specified in the grammar file
	char theTrace=0; //to trigger the creation of the trace file
	TVecteur dependency_display=DEPENDENCY_FUNCTION_DISPLAY; //DEPENDENCY_SENTENCE_NUMBER_DISPLAY,DEPENDENCY_INVERTED_DISPLAY,DEPENDENCY_FEATURE_DISPLAY,DEPENDENCY_INCOMPATIBLE_DISPLAY
	char outputXML=NO; //to output in an XML format
	char tagger=NO; //To only display the tagger results
    char tagging=YES;//To use tagging in the parsing
    char math=NO;//To display numerical variables on screen
	char display_error=NO;//To display warning messages
	char resultDisplay=YES;//To display result
	TVecteur dependencyOrder=DISPLAY_DEPENDENCY_BY_NAME; //DISPLAY_DEPENDENCY_BY_NODE,DISPLAY_DEPENDENCY_BY_CREATION
	TVecteur offsetDisplay=0; //to display the offsets on screen
	TVecteur displaymode= DISPLAY_LEMMA  | DISPLAY_CATEGORY | DISPLAY_CHUNK_TREE;
	TVecteur de_tree=DISPLAY_TREE; //DISPLAY_TREE_PHRASE,DISPLAY_TREE_COLUMN
	char dependency_extraction=YES; //to extract the dependency

	TRACEDEPENDANCE=NO;
	TRACETRAITS=NO;
	long conversionutf8=0; //UTF8_INPUT, DISPLAY_CONVERSION_UTF8
	SetColumn(80);

	int ipar=InitialisationParseur(language,
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
	GenereOptimise(NO);
	return ipar;
}

//=====================================================================
//=====================================================================
//=====================================================================
/*
This file contains a function that should be modified in the case of a new derivation of GlobalParseur
The function XipGrmFile is called from the user interface to load a grammar in memory.
The role of this function is to create a GlobalParseurNew object, which is then stored in gVecteurParseurs, when
InitialisationParseur is called to load the grammar in memory.


This function is called to load a GRM file. It should be modified to load your own grammar file
grmname is the name of your GRM file...
AS AN EXAMPLE, WE HAVE CREATED A GRM FILE WHICH CONTAINS ONLY 2 TYPES OF LINES:

Content of our GRM file:
------------------------------
path:
MyPath=c:\\XIP\\Test\\stub

grammar:
$MyPath\\english.xip

------------------------------

The LoadGrm function can be enriched with many other options and many more parameters...
In this current implementation, it only reads a grammar pathname and some environment variables...
The method: ResetEnvironmentVariables creates also the default: GRM_PATHNAME variable, which contains the
the GRM pathname.
*/
char LoadGrm(string grmname,string& grammar) {
	char bufread[1000];

	char crypte=NO;
	ifstream grm((char*)grmname.c_str(),modeOuverture);
    if (grm.fail()) {
        sprintf(bufread,"Cannot open: %s",grmname);
        send_error(strdup(bufread));
    }
	
	//This function is exported by the XIP library
	ResetEnvironmentVariables(grmname);
	

//----------------------------------------------------------------
    while (!grm.eof()) {
		//We read a line a trim it... This function also replaces the Environment variables
		//with their values...
        GrmTrim(grm,(uchar*)bufread,999,NO);
        
        if (!*bufread)
            break;

		//If it is a path (the definition of a path variable)
		if (!strcmp(bufread,"path:")) {
			ReadPathValue(grm);
			continue;
		}

		//We then read the grammar pathname
		if (!strcmp(bufread,"grammar:")) {
			if (grammar[0]) {
				send_error("A GRAMMAR file has already been provided");
			}
            
			GrmTrim(grm,(uchar*)bufread,999,YES);
            if (bufread[0]!=0) {
                if (strlen(bufread)>999) {
                    send_error("Pathname for GRAMMAR too long");
                }
                grammar=bufread;
            }
            continue;
        }
		//We then read the grammar pathname, gram in that case is equivalent to grammar
		//with an important modification: the grammar is encrypted.
		if (!strcmp(bufread,"gram:")) {
			if (grammar[0]) {
				send_error("A GRAMMAR file has already been provided");
			}
            crypte=YES;
			GrmTrim(grm,(uchar*)bufread,999,YES);
            if (bufread[0]!=0) {
                if (strlen(bufread)>999) {
                    send_error("Pathname for GRAMMAR too long");
                }
                grammar=bufread;
            }
            continue;
        }

		//You can add as many options as you like...
	}
	return crypte;
}

//================================================================================
//================================================================================


//THIS FUNCTION SHOULD ALWAYS BE IMPLEMENTED IN YOUR DERIVATION
Exportation int XipGrmFile(string grmname,char loadall,int xml) {

	//The loading should take place here...
	string grammar;
	string path;
	string name;
	
	//First we load the grammar name
	//LoadGrm could have as many parameters as you like
	char crypte=LoadGrm(grmname,grammar);
	//XIP exports a function to separate the path from the name... It is of course platform dependent
	CutPathFromName(grammar,path,name);

	GlobalParseurTry* parser=new GlobalParseurTry((char*)name.c_str(),(char*)path.c_str());
	
	//Then we load the grammar itself...
	VECTA<char*> parameters;
	int ipar=parser->loading((char*)path.c_str(),crypte,parameters);
    return ipar;
}


//IT IS ALSO MANDATORY TO REDEFINE YOUR WHOAMI STRING
Exportation void Whoami(string& question) {
	question="THIS IS MY OWN DERIVATION OF XIP";
}

//Comments:
//hmmselection is a pre-declared feature in XIP, which is used to mark the reading that had been selected, but not deleted, by the HMM.
#ifdef XIPDERIVATIONMAIN
int main(int argc, char* argv[])
{
	int ipar=XipGrmFile("R:\\XIPCVS\\parSem\\xip\\dev\\GRAMMARS\\ITALIAN\\GRMFILES\\italianmin.grm",1,0);
	GlobalParseur* parser=XipSelectGrammar(ipar);
	FILE* text=fopen("R:\\XIPCVS\\parSem\\xip\\dev\\GRAMMARS\\ITALIAN\\GRMFILES\\output.txt","r");
	//This instruction set the value of fichierDonnees with text
	parser->SetColumn(80);
	parser->FileAssociation(text);
	parser->Analyse(0,1);
	ostringstream os;
	XipParseStringOS("This is an example",ipar,&os);
	delete parser;
	return 0;
}
#endif
