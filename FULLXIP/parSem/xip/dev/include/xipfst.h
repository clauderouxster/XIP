#ifndef transduct_h
#define transduct_h
#include "kif.h"
#include "kifautomaton.h"



class xipfststate {
public:
	unsigned char status;
	hmap<unsigned long, xipfststate> arcs;

	xipfststate() {
		status = 0;
	}

};

class GlobalParseurXipFST: public GlobalParseurHMM {
public:
	KifAutomaton* ktrans;
	
	GlobalParseurXipFST(char* ftrans,char* hmm,char* filename,char* path);
	char lookup();
	~GlobalParseurXipFST();
	virtual typeParseur Type() { return PARSEURXIPFST; }
    void SentenceAnalysis(char renumerote,char tree);
    
    void Apply();
    

    void StringAssociation(unsigned char* chaine);
    FILE* FileAssociation(FILE* fichier);

    void AddAToken(unsigned char* tok);    
    void StartAnalysis();
    void CloseAnalysis();
    void AnalyseTokenFichier();
    void AnalyseTokenChaine();
}; 


#endif
