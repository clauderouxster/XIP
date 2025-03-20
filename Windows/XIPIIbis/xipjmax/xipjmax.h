#ifndef jmaxip_h
#define jmaxip_h

class GlobalParseurJMAX : public GlobalParseur {
 public:

    
    GlobalParseurJMAX(char* filename, char* taf4, char* lsc, char* tot,char* path);
    ~GlobalParseurJMAX();

    istream* donnees;

    ProcessorFactory procFactory;
    Processor* processor;
    Analyzer* azr;

    void AppendLemmaXipResult(char* buffer,int,int);

    void AssocieChaine(char* chaine) {
#if (defined(XIPSTLAPI) || defined(XIPSTLUSE))
        donnees = new istringstream(chaine);
#else
        donnees = new istrstream(chaine);
#endif
    }
    
    void AssocieStream(istream* is) {
        donnees=is;
    }

    void epr();
    void registerEntries(ListeVoc* voc,char* s);
    void Analyse(char renumerote,char tree);
    void AnalysePhrase(char renumerote,char tree);
};

#endif
