#ifndef client_h
#define client_h


class readingCharacter {
 public:

    int currentIndex;
    char ReadingBuffer[5000];
    int currentSize;
    int MaxSize;
    int readSoFar;
    int theSocket;
    int EOFILE;

    readingCharacter(int size,int re,int s) {
        currentIndex=0;
        currentSize=0;
        ReadingBuffer[0]=0;
        MaxSize=size;
        readSoFar=re;
        theSocket=s;
        EOFILE=0;
    };

    void DisplayXML();
    void ResultXML(XipResult& result);
    void LireUneBalise(char*,char init=0);
    unsigned char LireSuite();
    XipDependency* LireDependances(char* balise,VECTA<XipNode*>& lesNoeuds);
    XipNode* LireNoeud(char* balise,XipNode* pere,VECTA<XipNode*>& lesNoeuds);
    unsigned char LirePCDATA(char* pcdata);
};

    
#endif
