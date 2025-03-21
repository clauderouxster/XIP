#ifndef agde_h
#define agde_h

#include "level.h"
#include "xipstlres.h"
#include "automate.h"

int typePOS(string pos);

class PosMark {
	public:
	long cursor;
	string balise;
	string title;
	string newid;
	long start;
	char utilise;
	int pos;
	int sentence_number;
	PosMark(string n,long c) : balise(n) {
		cursor=c;
		utilise=0;
		pos=-1;
		sentence_number=-1;
	}
	
	PosMark(string n,string id,long s,long c) : balise(n),newid(id) {
		cursor=c;
		start=s;
		utilise=0;
		newid=id;
		start=s;
		pos=-1;
		sentence_number=-1;
	}
};

class agde {
	public:
	//Arbre des concepts
	//entities* gArbre;
	int gNouveau_Vecteur;
	int nbFichiers;
	//Ensemble des textes
	VECTA<levelText*> gTextes;
	VECTA<entities*> listeDesConcepts;
	VECTA<entities*> indexedConceptList;
	vecteur dependencyName;
	string current;
	void* lastmarkup;    
	int tailleresult;
	char newConceptAdded;
	VECTA<PosMark*> posmark;
	automate autoposmark;
	automate autoconcepts;
	char store_sentence;
	int posdebut;
	agde() {
		posdebut=0;
		newConceptAdded=1;
		store_sentence=0;
		CompteConcepts();
		CompteFichiers();
	}
	~agde();
	entities* aConcept(int id);
	char RestoreSentences(int idoc);
	void RestoreDependencies(levelText* level,int idoc,int ipar,int isent);
	void ReloadAllDocConcept();
	void ReloadAllConcepts();
	levelText* RestoreDocument(int idoc);
	void RestoreAllDocuments();
	char ResearchAndAdd(int iconc,string& word,int& pos,int& occ);
	void LoadDocumentName(string& pathname, int idoc);
	int ReloadDocConcept(entities* e);
	void SaveDocConcept();
	int LookForDocument(string filename);
	void store(int);
	void CompteConcepts();
	void CompteFichiers();
	int ResearchAndAdd(string& ,int&,int&);
	void saveConcepts(string filename);
	void saveDocuments(string filename,int);
	void reloadConcepts(string filename);
	levelText* reloadDocument(string filename);
	levelText* reloadDocument(int ref);
	void saveTableFile(ofstream& file);
	void reloadTableFile(ifstream& file);
	levelText* extractInformation(XipResult& xip,XipResult* coref,string idtext,int ipar,int calcul_semantique,levelText*,int first=-1,int last=-1);
	int extractInfoRequest(XipResult& xip, int numQuery, levelSentence& query,vecteur& focus,string&,char wordonly=0);
	entities* addConcept(string mot,vecteur* v);
	entities* concept(string mot, string pos,char lower=NO);
	entities* conceptQuestion(string mot, string pos,char lower=NO);
	char concept(int id);
	void addToConceptList(entities* a);
	void exploreNodes(XipNode* root,XipNode* mother,XipNode* node,levelDependency* dep,int num=-1,char stockeTraits=1);
	void exploreNodesQuestion(XipNode* mother,XipNode* node,levelDependency* dep,char onlyfeatures=0);
	granulet* CreeConcept();
	void removeDoc(int doc);
	void printEntities(vecteur& v,ostream& os);
	void returnCommonDocuments(vecteur& v, VECTA<int>& interdocs);
	char ExtractDependencies(string buffer,int ipar, VECTA<levelDependency*>& results);    
	void lookForCoref(XipNode* node, XipResult* xipcoref,VECTA<XipNode*>& NameLists,levelDependency* dep_vect,int numberDoc);
	void exploreNodesQuestionWithNoFeatures(XipNode* mother,XipNode* node,levelDependency* dep);
	int NewIDSentence();
	void storeconceptoffset();
	//void test();
};

#endif
