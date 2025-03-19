// derivation.cpp : Defines the entry point for the console application.
//

//Compilation flags: XIPDLL_EXPORT, XIPSTLAPI, XIPSTLUSE
//Libraries: libxips.lib, libpython.a

#include "parsbin.h"

class GlobalParseurTry : public GlobalParseur {
	public:
	void SentenceAnalysis(char renumerote,char tree);
	void LexiconEntryParsing(char* chaine,char** duplicat);
	void loading(char* path,VECTA<char*>& fileparam);
	GlobalParseurTry(char* grammarfile,char* pathname);
	int read(char*,int);
};

GlobalParseurTry::GlobalParseurTry(char* grammarfile,char* pathname):GlobalParseur(grammarfile,pathname) {}


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

void GlobalParseurTry::LexiconEntryParsing(char* reading_string,char** duplicat) {
	char* pt;
	
	//we reset the list of tags
	ListeTagsRAZ();
	
	//The number of reading
	nb_lectures=0;
	
	if (TestAffichage(AFFICHAGE_ENTREE) || DESAMBIGUISATION==OUI)
		*duplicat= DupliqueChaine(reading_string);
	else
		*duplicat=NULL;
	
	//We suppose the separator to be a "|" : surface|lemma|leftoffset|rightoffset|F1|F2|
	//The string also ends with a pipe in our example (to simplify the job)
	char* start=reading_string;
	pt=strchr(start,'|');
	int counter=0;
	while (pt!=NULL) {	
		//we split at the "|"
		*pt=0;
		ListeTagsAjouter(start);
		//The lemma form
		start=pt+1;
		pt=strchr(start,'|');
		//By default, the morphological tags from the external lexicons are in lowercase, execept if the NON_NORMALISATION_TAG is set.
		//We do not put in lowercase, the surface form and the lemma form, so we start testing after the second string.
		if (counter==2 && !TestAffichage(NON_NORMALISATION_TAG))
			MetEnMinuscule((uchar*)start);
		counter++;
	}
}

//Analyse d'une sequence de mots a partir d'un fichier...
void GlobalParseurTry::SentenceAnalysis(char renumerote,char arbre) {
	char buffer[1000];
	char* dupchaine;
	int bascule=1;
	baseOffset=0;
	char loop=1;
	
	while (loop) {        
		//if the renumbering is requested, we set the word_number to 0
		if (renumerote==OUI)
			num_mot=0;
		else //otherwise we increment it
			num_mot++;
		
		nombre_mots++;
		ListeVoc* voc=NULL;
		
		char testSentenceEnd=NON;
		char testMarkupTags=NON;
		
		char internalloop=1;
		loop=read(buffer,1000);
		while (loop) {
			switch(loop) {
				case 0:
				break;
				case 1:
				testMarkupTags=TestBalise(buffer);
				if (testMarkupTags==NON) {
					testSentenceEnd=EndOfSentence(buffer,NON);
					LexiconEntryParsing(buffer,&dupchaine);
					if (voc==NULL) {
						voc=ConstruitForme(dupchaine,num_mot);
						if (voc!=NULL) {
							num_mot++;
							nombre_mots++;
						}
					} else {
						FusionneForme(voc,dupchaine); 
					}
				}         
				break;
				case 2:
				voc=NULL;
				if (testMarkupTags==NON)
					gBaliseDebut->raz();
			}
			
			if (!loop || testSentenceEnd==OUI || EndOfSentence(buffer,UTILISE))
				break;
			loop=read(buffer,1000);
		}
		
		//we then parse the sentence
		Parse(OUI);
		//we increment the sentence number
		num_phrase++;
		//We then delete the intermediate representation
		Detruit();
	}
	
	//Analysis End reached
	FinAnalyse=OUI;
	while (phrase.dernier!=0) {
		//If there is still something to parse, we then parse again..
		Parse(OUI);
		Detruit();
	}
	FinAnalyse=NON;
}


void GlobalParseurTry::loading(char* path,VECTA<char*>& fileparam) {
	
	typeLangue language=ANGLAIS; //Do not touch, always English (Anglais)
	char stringfile[]=""; //useless, to replace the error messages and field names with new ones
	int numberValues=-1; // better when specified in the grammar file
	char theTrace=0; //to trigger the creation of the trace file
	TVecteur dependency_display=DEPENDENCY_FUNCTION_DISPLAY; //DEPENDENCY_SENTENCE_NUMBER_DISPLAY,DEPENDENCY_INVERTED_DISPLAY,DEPENDENCY_FEATURE_DISPLAY,DEPENDENCY_INCOMPATIBLE_DISPLAY
	char outputXML=NON; //to output in an XML format
	char tagger=NON; //To only display the tagger results
	char tagging=OUI;//To use tagging in the parsing
	char math=NON;//To display numerical variables on screen
	char crypte=NON;//To encrypt a gramamr
	char display_error=NON;//To display warning messages
	char resultDisplay=OUI;//To display result
	TVecteur dependencyOrder=DISPLAY_DEPENDENCY_BY_NAME; //DISPLAY_DEPENDENCY_BY_NODE,DISPLAY_DEPENDENCY_BY_CREATION
	TVecteur offsetDisplay=0; //to display the offsets on screen
	TVecteur displaymode= DISPLAY_LEMMA  | DISPLAY_CATEGORY | DISPLAY_CHUNK_TREE;
	TVecteur de_tree=DISPLAY_TREE; //DISPLAY_TREE_PHRASE,DISPLAY_TREE_COLUMN
	char dependency_extraction=OUI; //to extract the dependency
	
	TRACEDEPENDANCE=NON;
	TRACETRAITS=NON;
	long conversionutf8=0; //UTF8_INPUT, DISPLAY_CONVERSION_UTF8
	
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

int main(int argc, char* argv[])
{
	VECTA<char*> parameters;
	cout <<"init" <<endl;
	GlobalParseurTry* parser=new GlobalParseurTry("english.xip","toyGram");
	cout <<"loading" <<endl;
	parser->loading("toyGram",parameters);
	FILE* text=fopen("input.txt","r");
	//This instruction set the value of fichierDonnees with text
	parser->FileAssociation(text);
	cout <<"parse" <<endl;
	parser->Analyse(0,1);
	return 0;
}



//Comnments:
//hmmselection is a pre-declared feature in XIP, which is used to mark the reading that had been selected, but not deleted, by the HMM.
