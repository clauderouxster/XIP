#ifndef level_h
#define level_h

//On definit les differents levels d'analyse
// level dependance
// level phrase
// level paragraphe
// level texte
#include "vecteur.h"
#include "entities.h"

#include "mysql++.h"
using namespace mysqlpp;

typedef enum {NONE,TEXTLEVEL,PARAGRAPHLEVEL,SENTENCELEVEL,SUBGRAPHLEVEL,DEPENDENCYLEVEL} matchLevel;

class levelResult {
	public:
	matchLevel match;
	long left, right;
	vecteur* results;
	levelResult(matchLevel m=NONE,int l=-1,int r=-1)
	{
		match = m;
		left = l;
		right = r;
		results=NULL;
	}
	void init(matchLevel m,vecteur* res,int l,int r)
	{
		match = m;
		left = l;
		right = r;
		results=res;
	}
};

class level {
	public:
	vecteur key;
	long left,right;
	level() {left=-1;right=-1;};
	void append(vecteur* e) {
		key.merge(e);
	}
	void append(vecteur& e) {
		key.merge(e);
	}
	virtual char enrichSublevel(vecteur& filter, vecteur& adding) {return NO;};
	virtual void enrich(vecteur& adding) {};
	virtual char testSublevel(vecteur& value,VECTA<levelResult*>& offsets) {return NO;};
};

class levelDependency : public level{
	public:
	char type;
	void save(ofstream& file,char);
	char reload(ifstream& file, char* buffer,int max);
	void store(agde*,Query* query,int idoc,int ipar,int isent,int idep);
	char enrichSublevel(vecteur& filter, vecteur& adding);
	void enrich(vecteur& adding);
	char testSublevel(vecteur& value,VECTA<levelResult*>& offsets);
};

class levelSentence : public level{
	public:
	int num;    
	VECTA<levelDependency*> dependencies;
	VECTA<levelDependency*> subgraphes;
	void save(ofstream& file);
	void store(agde*,Query* query,int idoc,int ipar);
	void storeIndex(agde*,Query* query,int idoc,int ipar);
	void reload(ifstream& file, char* buffer,int max);
	char enrichSublevel(vecteur& filter, vecteur& adding);
	void enrich(vecteur& adding);
	char testSublevel(vecteur& value,VECTA<levelResult*>& offsets);
	levelSentence() {num=0;}
	~levelSentence() {
		dependencies.nettoie();
		subgraphes.nettoie();
	}
	
};

class levelParagraph : public level{
	public:
	VECTA<levelSentence*> sentences;
	void save(ofstream& file);
	void store(agde*,int idoc,int ipar);
	void storeOffsets(agde* ag,Query* query,int idoc,int ipar);
	void storeIndex(agde* ag,Query* query,int idoc,int ipar);
	void reload(ifstream& file, char* buffer,int max);
	char enrichSublevel(vecteur& filter, vecteur& adding);
	void enrich(vecteur& adding);
	char testSublevel(vecteur& value,VECTA<levelResult*>& offsets);
	~levelParagraph() {
		sentences.nettoie();
	}
};

class levelText : public level {
	public:
	string identity;
	int number;
	char loaded;
	int firstsentence;
	VECTA<levelParagraph*> paragraphes;
	levelText(int numberDoc,string id="");
	void store(agde*);
	void storeIndex(agde* ag);
	void save(ofstream& file);
	void reload(ifstream& file, char* buffer,int max);
	char enrichSublevel(vecteur& filter, vecteur& adding);
	void enrich(vecteur& adding);
	char testSublevel(vecteur& value,VECTA<levelResult*>& offsets);
	void storeIndexSentence();
	~levelText();
};

#endif 
